#include "server.h"

namespace http
{
	server::server(const char* address, const u_short port)
	{
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(2, 2), &wsa_data);
		this->sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		memset(&this->sa_, 0, sizeof(this->sa_));
		this->sa_.sin_family = AF_INET;
		this->sa_.sin_port = htons(port);
		inet_pton(AF_INET, address, &(this->sa_.sin_addr));
		const int len = strlen(address);
		this->address_ = new char[len + 1];
		memcpy(this->address_, address, sizeof(char) * (len + 1));
		this->port_ = port;
		bind(this->sock_, reinterpret_cast<SOCKADDR*>(&this->sa_), sizeof SOCKADDR);
	}

	void server::start(const int backlog)
	{
		listen(this->sock_, backlog);
		cout << "The server is listening at " << this->address_ << ":" << this->port_ << endl;

		int len = sizeof SOCKADDR;

		while (true)
		{
			SOCKADDR_IN client_sa;
			auto client = accept(this->sock_, reinterpret_cast<SOCKADDR*>(&client_sa), &len);
			if (client == INVALID_SOCKET)
			{
				cout << "Error occured: " << WSAGetLastError() << endl;
				exit(1);
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

	string tolower(string str)
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
		meta.method = tolower(trim(line.substr(0, first_space_pos)));
		meta.path = trim(line.substr(first_space_pos + 1, last_space_pos - first_space_pos - 1));
		meta.version = tolower(trim(line.substr(last_space_pos + 1)));
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
			const auto key = tolower(trim(line.substr(0, colon_pos)));
			const auto value = tolower(trim(line.substr(colon_pos + 1)));
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
				if (error_code == WSAETIMEDOUT)
					cout << "Receive aborted in thread " << std::this_thread::get_id() << " due to timeout" << endl;
				else
					cout << "Failed to received request in thread " << std::this_thread::get_id() <<  ". Error code: " << WSAGetLastError() << endl;
			}
			data += buffer;
		}
		return data;
	}

	int server::send_response(const SOCKET client, const response& resp)
	{
		const auto raw = resp.raw();
		const auto sent = send(client, raw.data(), raw.length(), 0);
		if (sent == SOCKET_ERROR)
			cout << "Failed to send response in thread " << std::this_thread::get_id() << ". Error code: " << WSAGetLastError() << endl;
		else
			cout << "Response sent in thread " << std::this_thread::get_id() << endl << "----- begin response -----" << endl << raw << "----- end response -----" << endl;
		return sent;
	}

	void server::client_accept(server* server, SOCKET client, SOCKADDR_IN client_sa)
	{
		char ip[INET6_ADDRSTRLEN];
		memset(ip, 0, sizeof ip);
		const UINT port = ntohs(client_sa.sin_port);
		inet_ntop(AF_INET, &client_sa.sin_addr, ip, INET6_ADDRSTRLEN);
		cout << "Created thread " << std::this_thread::get_id() << " for connection from " << ip << ":" << port << endl;
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
					cout << "Receive aborted in thread " << std::this_thread::get_id() << " due to timeout" << endl;
				else
					cout << "Failed to received request in thread " << std::this_thread::get_id() << ". Error code: " << WSAGetLastError() << endl;
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
			catch (exception& e)
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
		cout << "Connection closed in thread " << std::this_thread::get_id() << " for connection from " << ip << ":" << port << endl;
	}
}
