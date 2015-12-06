//
// Created by Bernd Lappas on 22.11.15.
//

#ifndef OPERATOR_FORWARDING_UI_OBJECT_CONTAINER_H
#define OPERATOR_FORWARDING_UI_OBJECT_CONTAINER_H

#include <ostream>

#include "ui_object.h"

using std::cout;
using std::string;
using std::vector;

//typedef vector<ui_object> used_container;
//using used_container = vector<string>;
using used_container = vector<ui_object>;

class ui_object_container
{
public:
//    typedef used_container::const_iterator const_iterator;
    using const_iterator = used_container::const_iterator;

    ui_object_container() : m_Childs()
    {
        cout << "ui_object_container(): " << std::hex << this << " Capacity: " << m_Childs.capacity() << "\n";
    }

    virtual ~ui_object_container()
    {
        cout << "~ui_object_container(): " << std::hex << this << "\n";
    }

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        m_Childs.emplace_back(std::forward<Args>(args)...);
    }

    template <class... Args>
    typename used_container::reference
    operator[](Args&&... args)
    {
        return m_Childs.operator[](std::forward<Args>(args)...);
    }

    template <class... Args>
    typename used_container::reference
    operator[](Args&&... args) const
    {
        return m_Childs.operator[](std::forward<Args>(args)...);
    }

    used_container::size_type size()
    {
        return m_Childs.size();
    }

    const_iterator begin() const
    {
        return m_Childs.begin();
    }

    const_iterator end() const
    {
        return m_Childs.end();
    }
private:
    used_container m_Childs;
};

void test(ui_object_container &ObjContainer)
{
}

void print_to(const ui_object_container &ObjContainer, ostream &out, size_t position)
{
    out << string(position, ' ') << "<document>\n";
    for (auto &e : ObjContainer)
    {
//        out << to_string(e.GetTag()) << ": ";
        geo::IntRect r = get_rect(e);
        print_to(e, out, position + 2);
    }
    out << string(position, ' ') << "</document>";
}

void draw(const ui_object_container &ObjContainer, ostream &out)
{
    for (auto &e : ObjContainer)
    {
        draw(e, out);
    }
}

#endif //OPERATOR_FORWARDING_UI_OBJECT_CONTAINER_H
