//
// Created by Bernd Lappas on 22.11.15.
//

#ifndef OPERATOR_FORWARDING_OBJECT_H
#define OPERATOR_FORWARDING_OBJECT_H

#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
//#include <memory>


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ostream>

#include "geo_rectangle.h"

using std::ostream;
using std::move;
using std::make_shared;
using std::shared_ptr;
using std::cout;

template<typename T> void print_to(const T &X, ostream &Out, size_t Position);
template<typename T> void draw(const T &X, ostream &Out, geo::IntRect Position);
//template<typename T> geo::IntRect get_rect(const T &X);

class ui_object
{
public:
    template<typename T>
    ui_object(T x) : m_Self(make_shared<model < T>>(move(x)))
    {
        cout << "ui_object(): " << std::hex << this << "\n";
    }

    virtual ~ui_object()
    {
        cout << "~ui_object(): " << std::hex << this << "\n";
    }

//    ui_object(const ui_object &obj) : m_Self(obj.m_Self)
//    {
//        cout << "ui_object() copy: " << std::hex << this << "\n";
//    }
//
//    ui_object(const ui_object &&obj) : m_Self(obj.m_Self)
//    {
//        cout << "ui_object() move: " << std::hex << this << "\n";
//    }

    void test()
    {

    }

    template<typename T>
    boost::uuids::uuid get_tag()
    {
        model<T> oc = m_Self.get();
//        return static_cast<model<T>>(m_Self).m_Tag;
        return oc.m_Tag;
    }

    geo::IntRect get_rect()
    {
        return m_Self->internal_get_rect();
    }

    friend void print_to(const ui_object &x, ostream &out, size_t position)
    {
        x.m_Self->internal_print_to(out, position);
    }

    friend void draw(const ui_object &x, ostream &out)
    {
        x.m_Self->internal_draw(out);
    }

    friend geo::IntRect get_rect(const ui_object &x)
    {
        return x.m_Self->internal_get_rect();
    }

private:
    struct object_concept
    {
        virtual ~object_concept() = default;

        virtual void internal_print_to(ostream &, size_t) const = 0;
        virtual void internal_draw(ostream &) const = 0;
        virtual geo::IntRect internal_get_rect() const = 0;
    };

    template<typename T>
    struct model : object_concept
    {
        model(T x) : m_Data(move(x)), m_Tag(boost::uuids::random_generator()())
        {
            cout << "model(): " << std::hex << this << " tag: " << m_Tag << "\n";
        }

        virtual ~model()
        {
            cout << "~model(): " << std::hex << this << " tag: " << m_Tag << "\n";
        }

        model(const model &obj) : m_Data(obj.m_Data), m_Tag(obj.m_Tag)
        {
            cout << "model() copy: " << std::hex << this << " tag: " << m_Tag << "\n";
        }

        model(const model &&obj) : m_Data(move(obj.m_Data)), m_Tag(obj.m_Tag)
        {
            cout << "model() move: " << std::hex << this << " tag: " << m_Tag << "\n";
        }

        void internal_print_to(ostream &out, size_t position) const
        {
            print_to(m_Data, out, position);
            out << " Tag: " << to_string(m_Tag) << "\n";
        }

        void internal_draw(ostream &out) const
        {
            draw(m_Data, out, m_Position);
        }

        geo::IntRect internal_get_rect() const
        {
            return m_Position;
        }

        T m_Data;
        boost::uuids::uuid m_Tag;
        geo::IntRect m_Position;
    };

    shared_ptr<const object_concept> m_Self;
};

#endif //OPERATOR_FORWARDING_OBJECT_H
