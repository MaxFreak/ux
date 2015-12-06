//
// Created by Bernd Lappas on 22.11.15.
//

#ifndef OPERATOR_FORWARDING_GEO_POINT_H
#define OPERATOR_FORWARDING_GEO_POINT_H

#include <ostream>
#include <string>

namespace geo
{
    using std::ostream;
    using std::string;

    template<typename T>
    class geo_point
    {
    public:

        geo_point();

        geo_point(T X, T Y);

        template<typename U>
        explicit geo_point(const geo_point<U> &Point);

        void print_to(ostream &Out, size_t Position) const
        {
            Out << string(Position, ' ') << "(" << m_X << "/" << m_Y << ")";
        }

        T m_X; /// X coordinate of the point
        T m_Y; /// Y coordinate of the point
    };

    template<typename T>
    geo_point<T> operator-(const geo_point<T> &Right);

    template<typename T>
    geo_point<T> &operator+=(geo_point<T> &Left, const geo_point<T> &Right);

    template<typename T>
    geo_point<T> &operator-=(geo_point<T> &Left, const geo_point<T> &Right);

    template<typename T>
    geo_point<T> operator+(const geo_point<T> &Left, const geo_point<T> &Right);

    template<typename T>
    geo_point<T> operator-(const geo_point<T> &Left, const geo_point<T> &Right);

    template<typename T>
    geo_point<T> operator*(const geo_point<T> &Left, T Right);

    template<typename T>
    geo_point<T> operator*(T Left, const geo_point<T> &Right);

    template<typename T>
    geo_point<T> &operator*=(geo_point<T> &Left, T Right);

    template<typename T>
    geo_point<T> operator/(const geo_point<T> &Left, T Right);

    template<typename T>
    geo_point<T> &operator/=(geo_point<T> &Left, T Right);

    template<typename T>
    bool operator==(const geo_point<T> &Left, const geo_point<T> &Right);

    template<typename T>
    bool operator!=(const geo_point<T> &Left, const geo_point<T> &Right);


    template<typename T>
    inline geo_point<T>::geo_point() : m_X(0), m_Y(0)
    {

    }

    template<typename T>
    inline geo_point<T>::geo_point(T X, T Y) : m_X(X), m_Y(Y)
    {

    }

    template<typename T>
    template<typename U>
    inline geo_point<T>::geo_point(const geo_point<U> &Point) : m_X(static_cast<T>(Point.m_X)),
                                                                m_Y(static_cast<T>(Point.m_Y))
    {
    }

    template<typename T>
    inline geo_point<T> operator-(const geo_point<T> &Right)
    {
        return geo_point<T>(-Right.m_X, -Right.m_Y);
    }

    template<typename T>
    inline geo_point<T> &operator+=(geo_point<T> &Left, const geo_point<T> &Right)
    {
        Left.m_X += Right.m_X;
        Left.m_Y += Right.m_Y;

        return Left;
    }

    template<typename T>
    inline geo_point<T> &operator-=(geo_point<T> &Left, const geo_point<T> &Right)
    {
        Left.m_X -= Right.m_X;
        Left.m_Y -= Right.m_Y;

        return Left;
    }

    template<typename T>
    inline geo_point<T> operator+(const geo_point<T> &Left, const geo_point<T> &Right)
    {
        return geo_point<T>(Left.m_X + Right.m_X, Left.m_Y + Right.m_Y);
    }


    template<typename T>
    inline geo_point<T> operator-(const geo_point<T> &Left, const geo_point<T> &Right)
    {
        return geo_point<T>(Left.m_X - Right.m_X, Left.m_Y - Right.m_Y);
    }

    template<typename T>
    inline geo_point<T> operator*(const geo_point<T> &Left, T Right)
    {
        return geo_point<T>(Left.m_X * Right, Left.m_Y * Right);
    }

    template<typename T>
    inline geo_point<T> operator*(T Left, const geo_point<T> &Right)
    {
        return geo_point<T>(Right.m_X * Left, Right.m_Y * Left);
    }

    template<typename T>
    inline geo_point<T> &operator*=(geo_point<T> &Left, T Right)
    {
        Left.m_X *= Right;
        Left.m_Y *= Right;

        return Left;
    }

    template<typename T>
    inline geo_point<T> operator/(const geo_point<T> &Left, T Right)
    {
        return geo_point<T>(Left.m_X / Right, Left.m_Y / Right);
    }

    template<typename T>
    inline geo_point<T> &operator/=(geo_point<T> &Left, T Right)
    {
        Left.m_X /= Right;
        Left.m_Y /= Right;

        return Left;
    }

    template<typename T>
    inline bool operator==(const geo_point<T> &Left, const geo_point<T> &Right)
    {
        return (Left.m_X == Right.m_X) && (Left.m_Y == Right.m_Y);
    }

    template<typename T>
    inline bool operator!=(const geo_point<T> &Left, const geo_point<T> &Right)
    {
        return (Left.m_X != Right.m_X) || (Left.m_Y != Right.m_Y);
    }

    typedef geo_point<int> IntPoint;
    typedef geo_point<unsigned int> UIntPoint;
    typedef geo_point<float> FloatPoint;

} // namespace geo

#endif //OPERATOR_FORWARDING_GEO_POINT_H
