#include <vector>
#include <string>

class Bundle
{
    public:
    std::vector<unsigned char> uc_DATA_p4kaku;
    std::string str_DATA_p4kaku;

    std::vector<unsigned char> uc_DATA_checkbox;
    std::string str_DATA_checkbox;

	std::vector<unsigned char> uc_DATA_checkmark;
	std::string str_DATA_checkmark;

	std::vector<unsigned char> uc_DATA_dirbox;
	std::string str_DATA_dirbox;

	std::vector<unsigned char> uc_DATA_dirsel;
	std::string str_DATA_dirsel;

	std::vector<unsigned char> uc_DATA_pon;
	std::string str_DATA_pon;

	std::vector<unsigned char> uc_DATA_eye;
	std::string str_DATA_eye;

	std::vector<unsigned char> uc_DATA_nircmdc;
	std::string str_DATA_nircmdc;

    Bundle();
};
