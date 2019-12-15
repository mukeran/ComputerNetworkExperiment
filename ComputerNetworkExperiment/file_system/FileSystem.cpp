// #include "FileSystem.h"

// void file_system::save_mail(mail* input_mail)
// {
// 	const string filepath = "..\\MailDataStorage\\";
	
// 	map<string, string> encoded_mail_data = input_mail->get_encoded_data();
// 	map<string, string>::iterator it;

// 	string encoded_uuid = encoded_mail_data["VVVJRA=="];
// 	string uuid = base64_encoder::base64_decode(encoded_uuid);
// 	mail_uuid->insert(uuid);

// 	std::ofstream fout;
// 	fout.open((filepath + uuid).c_str(), std::ios::out | std::ios::app);

	
// 	for (it = encoded_mail_data.begin(); it != encoded_mail_data.end(); ++it)
// 		fout << it->first << ' ' << it->second << '\n';

// 	fout.close();
// }

// set<string> file_system::get_uuid_list()
// {
// 	return *mail_uuid;
// }

// map<string, string> file_system::load_mail(const std::string& uuid)
// {
// 	map<string, string>* ret = new map<string, string>;
// 	string filepath = "..\\MailDataStorage\\" + uuid;
// 	std::ifstream fin;
// 	fin.open(filepath.c_str(), std::ios::in);

// 	while (!fin.eof())
// 	{
// 		string encoded_key, encoded_value;
// 		fin >> encoded_key >> encoded_value;

// 		if (encoded_key.empty())
// 			continue;
		
// 		string key, value;
// 		key = base64_encoder::base64_decode(encoded_key);
// 		value = base64_encoder::base64_decode(encoded_value);

// 		(*ret)[key] = value;
// 	}

// 	fin.close();

// 	return *ret;
// }


