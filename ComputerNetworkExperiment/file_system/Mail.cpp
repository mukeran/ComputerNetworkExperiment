// #include "Mail.h"

// string mail::uuid_generate()
// {
// 	const string uuid_chars = "0123456789abcdef";

// 	srand(time(NULL));
	
// 	string uuid = "";
// 	for (int i = 1; i <= 8; ++i)
// 		uuid += uuid_chars[rand() % 16];
// 	uuid += '-';
// 	for (int i = 1; i <= 4; ++i)
// 		uuid += uuid_chars[rand() % 16];
// 	uuid += '-';
// 	for (int i = 1; i <= 4; ++i)
// 		uuid += uuid_chars[rand() % 16];
// 	uuid += '-';
// 	for (int i = 1; i <= 16; ++i)
// 		uuid += uuid_chars[rand() % 16];
	
// 	return uuid;
// }

// void mail::data_input(const string& filename)
// {
// 	string key, value;
// 	string encoded_key, encoded_value;
// 	std::ifstream fin;
// 	fin.open(filename.c_str(), std::ifstream::in);

// 	string uuid = this->uuid_generate();
// 	string encoded_uuid;
// 	encoded_uuid = base64_encoder::base64_encode(reinterpret_cast<const unsigned char*>(uuid.c_str()), uuid.length());
// 	(*encoded_elements)["VVVJRA=="] = encoded_uuid;

// 	while (!fin.eof())
// 	{
// 		fin >> key >> value;
// 		encoded_key = base64_encoder::base64_encode(reinterpret_cast<const unsigned char*>(key.c_str()), key.length());
// 		encoded_value = base64_encoder::base64_encode(reinterpret_cast<const unsigned char*>(value.c_str()), value.length());
// 		(*encoded_elements)[encoded_key] = encoded_value;
// 	}

// 	fin.close();
// }

// map<string, string> mail::get_encoded_data()
// {
// 	return *(this->encoded_elements);
// }
