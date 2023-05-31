#ifndef STRING_HOLDER_H
#define STRING_HOLDER_H

#include "../binary_tree/associative_container.h"
#include "../avl_tree/avl_tree.h"

class string_holder
{
public:
    class string_comparer
    {
    public:
        int operator()(std::string const & x, std::string const & y) {
            return x.compare(y);
        }
    };
private:
    static string_holder *_instance;
public:
    static string_holder *get_instance()
    {
        if (_instance == nullptr)
        {
            _instance = new string_holder;
        }

        return _instance;
    }

private:

    associative_container<std::string, std::pair<std::string *, unsigned *>> *_pool;

private:

    string_holder()
    {
        _pool = new avl_tree<std::string, std::pair<std::string *, unsigned *>, string_comparer>;
    }

public:

    string_holder(string_holder const &) = delete;

    string_holder(string_holder &&) = delete;

    string_holder &operator=(string_holder const &) = delete;

    string_holder &operator=(string_holder &&) = delete;

    ~string_holder() noexcept
    {
        delete _pool;
    }

public:

    std::string *get_string(std::string const &key)
    {
        std::pair<std::string *, unsigned *> find_result;
        try
        {
            find_result = _pool->get(key);
        }
        catch (bs_tree<std::string, std::pair<std::string *, unsigned *>, string_comparer>::find_exception const &ex)
        {
            auto * string_to_insert = new std::string(key);
            unsigned * used = new unsigned();
            (*used) = 1;
            _pool->insert(key, std::move(std::pair<std::string *, unsigned *>(string_to_insert, used)));
            return _pool->get(key).first;
        }
        (*(find_result.second))++;
        return find_result.first;
    }

    void remove_string(std::string const &key)
    {
        std::pair<std::string *, unsigned *> string_and_used;
        try
        {
            string_and_used = _pool->get(key);
        }
        catch (bs_tree<std::string, std::string *, string_comparer>::find_exception const &ex)
        {
            // it cannot be (?)
            return;
        }

        if ((*(string_and_used.second)) == 1) {
            _pool->remove(key);
        } // todo:
        else {

        }
    }

};

inline string_holder *string_holder::_instance = nullptr;

#endif //STRING_HOLDER_H
