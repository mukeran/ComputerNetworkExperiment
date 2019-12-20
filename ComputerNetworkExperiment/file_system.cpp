#include "file_system.h"

file_system::file_system()
{
	if (_access(storage_path.data(), 0) == -1)
		_mkdir(storage_path.data());
}

void file_system::save_mail(mail* mail) const
{
	auto data = mail->get_kv();
	std::ofstream fs(storage_path + "\\" + mail->uuid, std::ios::out);
	for (auto it = data.begin(); it != data.end(); ++it)
		fs << utils::base64_encode(it->first) + " " + utils::base64_encode(it->second) + "\n";
	fs.close();
}

vector<mail> file_system::get_mail_list() const
{
	vector<mail> list;
	_finddata_t file_info;
	const auto handle = _findfirst((storage_path + "\\*").data(), &file_info);
	if (handle == -1)
		return list;
	do
	{
		if (!utils::is_uuid(file_info.name))
			continue;
		list.emplace_back(this->get_mail(file_info.name));
	} while (!_findnext(handle, &file_info));
	_findclose(handle);
	return list;
}

mail file_system::get_mail(const string& uuid) const
{
	const auto path = storage_path + "\\" + uuid;
	if (_access(path.data(), 0) == -1)
		throw std::exception("No such mail");
	std::ifstream fs(storage_path + "\\" + uuid, std::ios::in);
	map<string, string> fields;
	while (!fs.eof())
	{
		string line;
		getline(fs, line);
		std::stringstream ss(line);
		string key, value;
		ss >> key >> value;
		fields[utils::base64_decode(key)] = utils::base64_decode(value);
	}
	fs.close();
	mail mail(uuid, fields);
	return mail;
}
