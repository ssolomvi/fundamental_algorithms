#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

// TODO: allocate/deallocate with guard
// size_of_node function

#include <stack>
#include "associative_container.h"
//#include "../exception/not_implemented.h"
#include "logger.h"
#include "logger_holder.h"
#include "memory_base_class.h"
//#include "traversable.h"
#include <vector>

template<typename tkey, typename tvalue, typename tkey_comparer>
class binary_search_tree
        : public associative_container<tkey, tvalue>,
          protected logger_holder
{
protected:
    struct node
    {
        tkey key;
        tvalue value;
        node *left_subtree_address;
        node *right_subtree_address;
    };

public:
    class bst_exception final : public std::exception {
    private:
        std::string _message;

    public:
        bst_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }

    };


public:
#pragma region iterators
    class prefix_iterator final : private logger_holder
    {
        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;
    private:
        node *_current_node;
        std::stack<node *> _path;
        binary_search_tree<tkey, tvalue, tkey_comparer> const *_iterable_context;
    public:
//        explicit prefix_iterator(binary_search_tree<tkey, tvalue, tkey_comparer> const*, node *current_node);
        explicit prefix_iterator(node *current_node);

        bool operator==(prefix_iterator const &other) const;
        bool operator!=(prefix_iterator const &other) const;
        prefix_iterator& operator++();
        prefix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const;
    };

    class infix_iterator final
    {
    private:
        node *_current_node;
        std::stack<node *> _path;
        binary_search_tree<tkey, tvalue, tkey_comparer> const *_iterable_context;

    public:
        explicit infix_iterator(node *current_node);
        bool operator==(infix_iterator const &other) const;
        infix_iterator& operator++();
        infix_iterator operator++(int not_used);
        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const;
    };

    class postfix_iterator final
    {
    private:
        node *_current_node;
        std::stack<node *> _path;
        binary_search_tree<tkey, tvalue, tkey_comparer> const *_iterable_context;
    public:
        explicit postfix_iterator(node *current_node);
        bool operator==(postfix_iterator const &other) const;
        postfix_iterator &operator++();
        postfix_iterator operator++(int not_used);
        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const;
    };
#pragma endregion

protected:
#pragma region insertion-reading-removing
    class template_methods: public logger_holder
    {
    protected:
        binary_search_tree<tkey, tvalue, tkey_comparer> *_context_tree;

        enum inner_function_response {
            nothing_to_do,
        };

        enum was_found_find_a_node_place_function_response {
            found,
            not_found
        };

        enum direction_find_a_node_place_function_response {
            left_subtree,
            right_subtree,
            root
        };

    protected:
        virtual void find_a_node_place(tkey const &key,
                               node *&tree_root_address,
                               std::stack<node **> &path_to_subtree_root_exclusive,
                               was_found_find_a_node_place_function_response *is_found,
                               direction_find_a_node_place_function_response *direction);
    };

#pragma region Insert template method
    class insertion_template_method: public template_methods
    {
    public:
        virtual ~insertion_template_method() = default;

    public:
        void insert(tkey const &key, tvalue &&value, node *&tree_root_address,
                    binary_search_tree<tkey, tvalue, tkey_comparer> * context_tree);

    private:
        void insert_inner(tkey const &key,
                tvalue &&value,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

    protected:
        virtual void before_insert_inner(
                tkey const &key,
                tvalue &&value,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

        virtual void after_insert_inner(
                tkey const &key,
                tvalue &&value,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive,
                typename template_methods::inner_function_response response);

        virtual void initialize_memory_with_node(tkey const &key,
                                                 tvalue &&value,
                                                 binary_search_tree<tkey, tvalue, tkey_comparer>::node * const node_address);
    };

#pragma endregion

#pragma region Read template method
    class reading_template_method : public template_methods
    {
    public:
        virtual ~reading_template_method() = default;

    public:
        tvalue const &read(
                tkey const &key,
                node *&tree_root_address);

    private:
        tvalue const &read_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

    protected:
        virtual void before_read_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

        virtual void after_read_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

    };
#pragma endregion

#pragma region Remove template method
    class removing_template_method : public template_methods
    {
    public:
        virtual ~removing_template_method() = default;

    public:
        tvalue &&remove(
                tkey const &key,
                node *&tree_root_address);

    private:
        tvalue &&remove_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

    protected:
        virtual void before_remove_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

        virtual void after_remove_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<node **> &path_to_subtree_root_exclusive);

    };
#pragma endregion
#pragma endregion

private:

    node *_root;
    memory *_allocator;
    logger *_logger;
    insertion_template_method *_insertion;
    reading_template_method *_reading;
    removing_template_method *_removing;

protected:
    explicit binary_search_tree(
            insertion_template_method *insertion,
            reading_template_method *reading,
            removing_template_method *removing,
            memory *allocator = nullptr,
            logger *logger = nullptr);

public:

    explicit binary_search_tree(memory *allocator = nullptr, logger *logger = nullptr);

    binary_search_tree(binary_search_tree const &other); // copying?

    binary_search_tree(binary_search_tree &&other) noexcept; // wtf??

    binary_search_tree &operator=(binary_search_tree const &other); // assignment??

    binary_search_tree &operator=(binary_search_tree &&other) noexcept; // wtf??

    ~binary_search_tree();

public:

    void insert(tkey const &key, tvalue &&value) final;

    tvalue const &get(tkey const &key) final;

    tvalue &&remove(tkey const &key) final;

private:
    logger *get_logger() const noexcept override;

//    static std::string get_typename() noexcept;

public:
    prefix_iterator begin_prefix() const noexcept;

    prefix_iterator end_prefix() const noexcept;

    infix_iterator begin_infix() const noexcept;

    infix_iterator end_infix() const noexcept;

    postfix_iterator begin_postfix() const noexcept;

    postfix_iterator end_postfix() const noexcept;
};

#endif //BINARY_SEARCH_TREE_H
