#include "server.h"

namespace http
{
	server::server(const string& address, const u_short port)
	{
		this->sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		memset(&this->sa_, 0, sizeof(this->sa_));
		this->sa_.sin_family = AF_INET;
		this->sa_.sin_port = htons(port);
		inet_pton(AF_INET, address.data(), &(this->sa_.sin_addr));
		this->address_ = address;
		this->port_ = port;
		bind(this->sock_, reinterpret_cast<SOCKADDR*>(&this->sa_), sizeof SOCKADDR);
	}

	void server::start(const int backlog)
	{
		listen(this->sock_, backlog);
		logger::info("The web server is listening at " + this->address_ + ":" + std::to_string(this->port_));

		int len = sizeof SOCKADDR;

		while (true)
		{
			SOCKADDR_IN client_sa;
			auto client = accept(this->sock_, reinterpret_cast<SOCKADDR*>(&client_sa), &len);
			if (client == INVALID_SOCKET)
			{
				logger::error("Error occured: " + std::to_string(WSAGetLastError()));
				exit(3);
			}
			thread t(client_accept, this, client, client_sa);
			t.detach();
		}
	}

	string trim(string str)
	{
		while (str.length() && (*str.begin() == ' ' || *str.begin() == '\r' || *str.begin() == '\n')) str.erase(str.begin());
		while (str.length() && (*str.rbegin() == ' ' || *str.begin() == '\r' || *str.begin() == '\n')) str.erase(--str.end());
		return str;
	}

	string to_lower(string str)
	{
		for (auto& c : str) c = std::tolower(c);
		return str;
	}

	meta server::parse_meta(const string& line)
	{
		meta meta;
		const auto first_space_pos = line.find_first_of(' ');
		const auto last_space_pos = line.find_last_of(' ');
		if (first_space_pos == string::npos || last_space_pos == string::npos)
		{
			throw exception("Invalid HTTP request");
		}
		meta.method = to_lower(trim(line.substr(0, first_space_pos)));
		meta.path = trim(line.substr(first_space_pos + 1, last_space_pos - first_space_pos - 1));
		meta.version = to_lower(trim(line.substr(last_space_pos + 1)));
		return meta;
	}
	
	kv server::parse_header(const string& data)
	{
		auto rest = data;
		kv headers;
		while (rest.length() != 0)
		{
			const auto split_pos = rest.find("\r\n");
			string line;
			if (split_pos != string::npos)
				line = rest.substr(0, split_pos), rest = rest.substr(split_pos + 2);
			else
				line = rest, rest = "";
			line = trim(line);
			if (line.length() == 0)
				continue;
			const auto colon_pos = line.find_first_of(':');
			const auto key = to_lower(trim(line.substr(0, colon_pos)));
			const auto value = to_lower(trim(line.substr(colon_pos + 1)));
			headers[key] = value;
		}
		headers["raw"] = data;
		return headers;
	}

	string server::receive(const SOCKET client, const u_int size)
	{
		char buffer[max_buffer_size + 1];
		string data;
		while (data.length() < size)
		{
			const auto rest = size - data.length();
			const auto prefer = min(rest, max_buffer_size);
			memset(buffer, 0, sizeof(char) * prefer);
			setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&recv_timeout), sizeof(int));
			const auto received = recv(client, buffer, prefer, 0);
			if (received <= 0) {
				const auto error_code = WSAGetLastError();
				const auto thread_id = utils::get_thread_id();
				if (error_code == WSAETIMEDOUT)
					logger::warning("Receive aborted in thread " + std::to_string(thread_id) + " due to timeout");
				else
					logger::error("Failed to received request in thread " + std::to_string(thread_id) + ". Error code: " + std::to_string(error_code));
			}
			data += buffer;
		}
		return data;
	}

	int server::send_response(const SOCKET client, const response& resp)
	{
		const auto raw = resp.raw();
		const auto sent = send(client, raw.data(), raw.length(), 0);
		const auto thread_id = utils::get_thread_id();
		if (sent == SOCKET_ERROR)
			logger::error("Failed to send response in thread " + std::to_string(thread_id) + ". Error code: " + std::to_string(WSAGetLastError()));
		else
			logger::info("Response sent in thread " + std::to_string(thread_id));
		return sent;
	}

	void server::client_accept(server* server, SOCKET client, SOCKADDR_IN client_sa)
	{
		char ip[INET6_ADDRSTRLEN];
		memset(ip, 0, sizeof ip);
		const UINT port = ntohs(client_sa.sin_port);
		inet_ntop(AF_INET, &client_sa.sin_addr, ip, INET6_ADDRSTRLEN);
		const auto thread_id = utils::get_thread_id();
		logger::info("Created thread " + std::to_string(thread_id) + " for connection from " + ip + ":" + std::to_string(port));
		char buffer[max_buffer_size + 1];
		string data;
		while (true)
		{
			memset(buffer, 0, sizeof buffer);
			setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&recv_timeout), sizeof(int));
			const auto received = recv(client, buffer, max_buffer_size, 0);
			if (received <= 0) {
				const auto error_code = WSAGetLastError();
				if (error_code == WSAETIMEDOUT)
					logger::warning("Receive aborted in thread " + std::to_string(thread_id) + " due to timeout");
				else
					logger::error("Failed to received request in thread " + std::to_string(thread_id) + ". Error code: " + std::to_string(error_code));
				break;
			}
			data += buffer;
			const auto split_pos = data.find("\r\n\r\n");
			if (split_pos == string::npos)
			{
				if (data.length() > max_header_size) {
					send_response(client, response(kv{ {"Connection", "Close"} }, "", status_code::bad_request));
					break;
				}
				continue;
			}
			auto border_offset = split_pos + 4;
			
			const auto first_break_pos = data.find("\r\n");
			auto meta = parse_meta(data.substr(0, first_break_pos));
			auto headers = parse_header(data.substr(first_break_pos + 2, split_pos));
			string body;
			if (meta.method != "get" && meta.method != "post")
			{
				send_response(client, response(kv{ {"Connection", "Close"} }, "", status_code::method_not_allowed));
				break;
			}
			if (meta.version != "http/1.1")
			{
				send_response(client, response(kv{ {"Connection", "Close"} }, "", status_code::http_version_not_supported));
				break;
			}
			if (meta.method == "post" && headers.count("content-length"))
			{
				const auto b_total = static_cast<unsigned>(stoi(headers["content-length"]));
				if (b_total > max_content_length)
				{
					send_response(client, response(kv{ {"Connection", "Close"} }, "", status_code::bad_request));
					break;
				}
				const auto b_received = data.length() - (split_pos + 4);
				if (b_received < b_total)
				{
					const auto b_rest = b_received - b_total;
					const auto b_data = receive(client, b_rest);
					if (b_data.length() != b_rest)
					{
						send_response(client, response(kv{{"Connection", "Close"}}, "", status_code::bad_request));
						break;
					}
					data += b_data;
				}
				body = data.substr(split_pos + 4, b_total);
				border_offset += b_total;
			}
			request *req;
			response resp;
			try
			{
				req = new request(meta.method, meta.path, meta.version, headers, body);
			}
			catch (exception&)
			{
				send_response(client, response(kv{ {"Connection", "Close"} }, "", status_code::bad_request));
				break;
			}
			handler::handle(*req, &resp);
			delete req;
			const auto sent = send_response(client, resp);
			if (sent == SOCKET_ERROR)
				break;
			if (resp.get_header("Connection") != "Keep-Alive")
				break;
			data = data.substr(border_offset);
		}
		closesocket(client);
		logger::info("Connection closed in thread " + std::to_string(thread_id) + " for connection from " + ip + ":" + std::to_string(port));
	}
}
