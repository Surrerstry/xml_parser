/*
g++ -std=c++17 main.cpp -o simple_xml_parser
./simple_xml_parser 1.xml tag1.tag2~name __tag1
*/

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <bits/stdc++.h>
#include <unordered_map>
#include <fstream>

using namespace std;

string strip(string str) {

	int remove_from_begin = 0, remove_from_end = 0;

	for(auto sign: str) {
		if(!isspace(sign)) {
			break;
		}
		remove_from_begin += 1;
	}
	reverse(str.begin(), str.end());

	for(auto sign: str) {
		if(!isspace(sign)) {
			break;
		}
		remove_from_end += 1;
	}
	reverse(str.begin(), str.end());

	return str.substr(remove_from_begin, (str.length() - 0) - remove_from_end);
	}

vector<string> split(string str, char separator) {
	vector<string> res;

	string tmp;
	str += separator;
	for(auto sign: str) {
		if(sign == separator) {
			res.push_back(tmp);
			tmp = "";
		}
		else {
			tmp += sign;
		}
	}

	return res;
	}


int main(int argc, char *argv[]) {

	if(argc == 1) {

		cout << "Usage: " << endl;
		cout << argv[0] << " File.xml tag1~value tag1.tag2~name ..." << endl;

		return 1;
	}

	ifstream xml_file(argv[1]);

	int hrml_amount = 0;

	vector<string> hrml_lines; 
	vector<string> queries_lines;

	string line;
	if(xml_file.is_open()) {

		bool in_value=false;
		while(getline(xml_file, line)) {
			vector<int> list_of_scopes;
	
				for(int pos=0; pos<line.length(); ++pos) {
					if(line[pos] == '\"' && in_value == false) {
						in_value = true;
					} else if(line[pos] == '\"' && in_value == true) {
						in_value = false;
					}
					
					if(in_value) {
						continue;
					}
	
					if(line[pos] == '<') {
						list_of_scopes.push_back(pos);
					} else if(line[pos] == '>') {
						list_of_scopes.push_back(pos+1);
					}
				}
	
			for(int i=0; i < list_of_scopes.size(); ++i) {
				string line_to_save;
				line_to_save = line.substr(list_of_scopes[i], list_of_scopes[i+1] - list_of_scopes[i]);
				if(line_to_save.length() > 0) {
					hrml_lines.push_back(line_to_save);
				}
			}
			if(list_of_scopes.size() == 0) {
				hrml_lines.push_back(line);
			}
	
		}
		xml_file.close();
	} else  {
		cout << "Unable to read file:" << argv[1] << endl;
		return 2;
	}

	for(int i=2; i<argc; ++i) {
		queries_lines.push_back(argv[i]);
		}

	vector<string> result;

	string tmp_string_with_current_path = "";
	string tmp_string_with_current_path_with_field_name = "";

	unordered_map<string, string> results;
	for(auto hrml_line: hrml_lines) {
		string current_tag = "";
		vector<string> current_values;

		smatch result;
		smatch result_2;
		regex pattern_for_opening_tag("<\\w* ");
		regex pattern_for_opening_tag_2("<\\w*>");

		regex_search(hrml_line, result, pattern_for_opening_tag);
		regex_search(hrml_line, result_2, pattern_for_opening_tag_2);
		if(result[0].length() > result_2[0]) {
			current_tag = result[0];
		} else {
			current_tag = result_2[0];
		}

		// opening tag
		if(current_tag.length() != 0) {
			current_tag = current_tag.substr(1, current_tag.length()-2);
			tmp_string_with_current_path += "."+current_tag;

			if (strip(hrml_line).length() == (current_tag.length() + 2) ) {
				0;
				// tag without values
			} else {

			string tmp_string_with_values = hrml_line.substr(current_tag.length() + 2, hrml_line.length() - (current_tag.length() + 3));
			// cout << "search values in:" << tmp_string_with_values << endl;

			bool tmp_name_switch = true;
			bool tmp_value_switch = false;
			int skip_next = 0;
			string tmp_name = "";
			string tmp_value = "";
			
			// cout << "current path:" << tmp_string_with_current_path << endl;
			for(auto sign: tmp_string_with_values) {
				if (skip_next) {
					skip_next--;
					continue;
				}

				if (tmp_name_switch) {
					tmp_name += sign;
				
				if (sign == ' ') {
					tmp_name_switch = false;
					tmp_value_switch = true;
					skip_next = 3;
					tmp_string_with_current_path_with_field_name = tmp_string_with_current_path+"~"+tmp_name;
					if (tmp_string_with_current_path_with_field_name[0] = '.') {
						tmp_string_with_current_path_with_field_name = tmp_string_with_current_path_with_field_name.substr(1, tmp_string_with_current_path_with_field_name.length()-1);
					}
					tmp_name = "";
					continue;
					}
				}

				if (tmp_value_switch) {
					tmp_value += sign;

					if (sign == '"') {
						tmp_string_with_current_path_with_field_name = strip(tmp_string_with_current_path_with_field_name);
						results[tmp_string_with_current_path_with_field_name] = tmp_value.substr(0, tmp_value.length() - 1);
						tmp_string_with_current_path_with_field_name = "";
						tmp_value = "";
						tmp_name_switch = true;
						tmp_value_switch = false;
						skip_next = 1;
					}
				}

			}

			}
		} else {
			// try to find closing tag
			regex pattern_for_closing_tag("</.*>");
			regex_search(hrml_line, result, pattern_for_closing_tag);
			current_tag = result[0];

			if(current_tag.length() !=  0) {
				current_tag = current_tag.substr(2, current_tag.length()-3);
				//cout << "Closing tag:" << current_tag << endl;
				if(tmp_string_with_current_path[0] == '.') {
					tmp_string_with_current_path = tmp_string_with_current_path.substr(1, tmp_string_with_current_path.length() - 1);
				}
				vector<string> splitted_tags;
				splitted_tags = split(tmp_string_with_current_path, '.');
				tmp_string_with_current_path = "";
				for(int i=splitted_tags.size()-1; i >= 0; --i) {
					if(splitted_tags[i] != current_tag) {
						tmp_string_with_current_path = "."+splitted_tags[i] + tmp_string_with_current_path;
						}
					}

			} else {
				// Not closing and not opening tag:
				string tmp_string_with_current_path_in_between = "";
				if(tmp_string_with_current_path[0] == '.') {
					tmp_string_with_current_path_in_between = tmp_string_with_current_path.substr(1, tmp_string_with_current_path.length() - 1);
				} else {
					tmp_string_with_current_path_in_between = tmp_string_with_current_path;
				}

				string key_to_save = "__" + tmp_string_with_current_path_in_between;
				results[key_to_save] = hrml_line;
			}


		}
		// cout << "IN BETWEEN:" << tmp_string_with_current_path << endl;
	}
	
	for(auto queries_line: queries_lines) {
		string tmp_res = results[queries_line];
		if(tmp_res.length() == 0) {
			cout << "Not Found!" << endl;
		} else {
			cout << tmp_res << endl;
		}
	}

	return 0;
}