//
// Created by Bernd Lappas on 22.11.15.
//

#ifndef OPERATOR_FORWARDING_GEO_RECTANGLE_H
#define OPERATOR_FORWARDING_GEO_RECTANGLE_H

#include <algorithm>
#include "geo_point.h"

namespace geo
{
    template<typename T>
    class geo_rectangle
    {
    public:

        geo_rectangle();

        geo_rectangle(T Left, T Top, T Width, T Height);

        geo_rectangle(const geo_point<T> &Position, const geo_point<T> &Size);

        template<typename U>
        explicit geo_rectangle(const geo_rectangle<U> &Rectangle);

        void print_to(ostream &Out, size_t Pos) const
        {
            Out << string(Pos, ' ') << "(" << m_Left << "/" << m_Top << "):(W " << m_Width << ", H " << m_Height << ")";
        }

        bool contains(T X, T Y) const;

        bool contains(const geo_point<T> &Point) const;

        bool intersects(const geo_rectangle<T> &Rectangle) const;

        bool intersects(const geo_rectangle<T> &Rectangle, geo_rectangle<T> &Intersection) const;

        T get_x1() const {return m_Left;};
        T get_y1() const {return m_Top;};
        T get_x2() const {return m_Left + m_Width;};
        T get_y2() const {return m_Top + m_Height;};
        T get_width() const {return m_Width;};
        T get_height() const {return m_Height;};

        T m_Left;   /// Left coordinate of the Rectangle
        T m_Top;    /// Top coordinate of the Rectangle
        T m_Width;  /// Width of the Rectangle
        T m_Height; /// Height of the Rectangle
    };

    template<typename T>
    bool operator==(const geo_rectangle<T> &Left, const geo_rectangle<T> &Right);

    template<typename T>
    bool operator!=(const geo_rectangle<T> &Left, const geo_rectangle<T> &Right);

    template<typename T>
    geo_rectangle<T>::geo_rectangle() : m_Left(0), m_Top(0), m_Width(0), m_Height(0)
    {
    }

    template<typename T>
    geo_rectangle<T>::geo_rectangle(T Left, T Top, T Width, T Height) : m_Left(Left), m_Top(Top), m_Width(Width),
                                                                        m_Height(Height)
    {
    }

    template<typename T>
    geo_rectangle<T>::geo_rectangle(const geo_point<T> &Position, const geo_point<T> &Size) : m_Left(Position.m_X),
                                                                                              m_Top(Position.m_Y),
                                                                                              m_Width(Size.m_X),
                                                                                              m_Height(Size.m_Y)
    {
    }

    template<typename T>
    template<typename U>
    geo_rectangle<T>::geo_rectangle(const geo_rectangle<U> &Rectangle) : m_Left(static_cast<T>(Rectangle.m_Left)),
                                                                         m_Top(static_cast<T>(Rectangle.m_Top)),
                                                                         m_Width(static_cast<T>(Rectangle.m_Width)),
                                                                         m_Height(static_cast<T>(Rectangle.m_Height))
    {
    }

    template<typename T>
    bool geo_rectangle<T>::contains(T X, T Y) const
    {
        // Rectangles with negative dimensions are allowed, so we must handle them correctly

        // Compute the real min and max of the rectangle on both axes
        T minX = std::min(m_Left, static_cast<T>(m_Left + m_Width));
        T maxX = std::max(m_Left, static_cast<T>(m_Left + m_Width));
        T minY = std::min(m_Top, static_cast<T>(m_Top + m_Height));
        T maxY = std::max(m_Top, static_cast<T>(m_Top + m_Height));

        return (X >= minX) && (X < maxX) && (Y >= minY) && (Y < maxY);
    }

    template<typename T>
    bool geo_rectangle<T>::contains(const geo_point<T> &Point) const
    {
        return contains(Point.m_X, Point.m_Y);
    }

    template<typename T>
    bool geo_rectangle<T>::intersects(const geo_rectangle<T> &Rectangle) const
    {
        geo_rectangle<T> intersection;
        return intersects(Rectangle, intersection);
    }

    template<typename T>
    bool geo_rectangle<T>::intersects(const geo_rectangle<T> &Rectangle, geo_rectangle<T> &Intersection) const
    {
        // Rectangles with negative dimensions are allowed, so we must handle them correctly

        // Compute the min and max of the first rectangle on both axes
        T r1MinX = std::min(m_Left, static_cast<T>(m_Left + m_Width));
        T r1MaxX = std::max(m_Left, static_cast<T>(m_Left + m_Width));
        T r1MinY = std::min(m_Top, static_cast<T>(m_Top + m_Height));
        T r1MaxY = std::max(m_Top, static_cast<T>(m_Top + m_Height));

        // Compute the min and max of the second rectangle on both axes
        T r2MinX = std::min(Rectangle.m_Left, static_cast<T>(Rectangle.m_Left + Rectangle.m_Width));
        T r2MaxX = std::max(Rectangle.m_Left, static_cast<T>(Rectangle.m_Left + Rectangle.m_Width));
        T r2MinY = std::min(Rectangle.m_Top, static_cast<T>(Rectangle.m_Top + Rectangle.m_Height));
        T r2MaxY = std::max(Rectangle.m_Top, static_cast<T>(Rectangle.m_Top + Rectangle.m_Height));

        // Compute the intersection boundaries
        T interLeft = std::max(r1MinX, r2MinX);
        T interTop = std::max(r1MinY, r2MinY);
        T interRight = std::min(r1MaxX, r2MaxX);
        T interBottom = std::min(r1MaxY, r2MaxY);

        // If the intersection is valid (positive non zero area), then there is an intersection
        if ((interLeft < interRight) && (interTop < interBottom))
        {
            Intersection = geo_rectangle<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
            return true;
        }
        else
        {
            Intersection = geo_rectangle<T>(0, 0, 0, 0);
            return false;
        }
    }

    template<typename T>
    inline bool operator==(const geo_rectangle<T> &Left, const geo_rectangle<T> &Right)
    {
        return (Left.m_Left == Right.m_Left) && (Left.m_Width == Right.m_Width) && (Left.m_Top == Right.m_Top) &&
            (Left.m_Height == Right.m_Height);
    }

    template<typename T>
    inline bool operator!=(const geo_rectangle<T> &Left, const geo_rectangle<T> &Right)
    {
        return !(Left == Right);
    }

    typedef geo_rectangle<int> int_rect;
    typedef geo_rectangle<float> float_rect;

} // namespace geo

#endif //OPERATOR_FORWARDING_GEO_RECTANGLE_H
