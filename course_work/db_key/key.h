#ifndef KEY_H
#define KEY_H

#include <iostream>
#include <fstream>

class key
{
    int _applicant_id{};
    int _contest_id{};

    friend class key_comparer;
    friend std::ostream &operator<<(std::ostream &out, const key &_key_);

public:
    class create_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit create_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

public:
    key(std::ifstream* input_stream, bool is_cin)
    {
        std::string token, delimiter = ", ";
        std::string applicant_id, contest_id;
        size_t pos;
        unsigned delimiter_length = delimiter.length();

        if (is_cin) {
            std::cout << "Enter key:\napplicant id, contest id: >>";
            std::getline(std::cin, token);
        } else {
            std::getline((*input_stream), token);
        }

        if ((pos = token.find(delimiter)) != std::string::npos) {
            applicant_id = token.substr(0, pos);
            token.erase(0, pos + delimiter_length);
        } else {
            throw key::create_exception("key constructor: incorrect input for key");
        }

        if (token.empty()) {
            throw key::create_exception("key constructor: incorrect input for key");
        }

        contest_id = token;

        this->_applicant_id = stoi(applicant_id);
        this->_contest_id = stoi(contest_id);
        // to get the time as a string: dt = asctime(new_key.utc_time);
    }

    key() = default;

    key(int applicant_id, int contest_id)
    : _contest_id(contest_id), _applicant_id(applicant_id)
    {

    }

    // copy constructor
    key(key const &obj)
    : key(obj._applicant_id, obj._contest_id)
    {

    }

    // move constructor
    key(key &&obj) noexcept
    : key(obj._applicant_id, obj._contest_id)
    {
        obj._applicant_id = 0;
        obj._contest_id = 0;
    }

    // copy assignment (оператор присваивания)
    key &operator=(key const &obj)
    {
        if (this == &obj)
        {
            return *this;
        }

        _applicant_id = obj._applicant_id;
        _contest_id = obj._contest_id;

        return *this;
    }

    // move assignment (оператор присваивания перемещением)
    key &operator=(key &&obj) noexcept
    {
        if (this == &obj)
        {
            return *this;
        }

        _applicant_id = obj._applicant_id;
        obj._applicant_id = 0;

        _contest_id = obj._contest_id;
        obj._contest_id = 0;

        return *this;
    }

    ~key() = default;
};

inline std::ostream &operator<<(std::ostream &out, const key &_key_)
{
    out << _key_._applicant_id << ", " << _key_._contest_id << std::endl;
    return out;
}

class key_comparer
{
public:
    int operator()(key x, key y) {
        int applicant_id_comparison = x._applicant_id - y._applicant_id;
        if (applicant_id_comparison == 0) {
            return x._contest_id - y._contest_id;
        } else {
            return applicant_id_comparison;
        }
    }
};

#endif //KEY_H
