//---------------------------------------------------------------------------
//
// CPSC453 -- Introduction to Computer Graphics
// Assignment 2
//
// Classes and functions for manipulating points, vectors, matrices, 
// and colours.  You probably won't need to modify anything in these
// two files.
//
// Adapted from CS488 A2 University of Waterloo Computer Graphics Lab / 2003
//
//---------------------------------------------------------------------------

#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679
#endif

class Point2D
{
public:
  Point2D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
  }
  Point2D(double x, double y)
  { 
    v_[0] = x;
    v_[1] = y;
  }
  Point2D(const Point2D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
  }

  Point2D& operator =(const Point2D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

private:
  double v_[2];
};

class Point3D
{
public:
  Point3D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
  }
  Point3D(double x, double y, double z)
  { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
  }
  Point3D(const Point3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
  } 

  Point3D& operator =(const Point3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

private:
  double v_[3];
};

class Point4D
{
public:
  Point4D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
    v_[3] = 1.0;
  }
  Point4D(double x, double y, double z, double w)
  {
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
    v_[3] = w;
  }
  Point4D(const Point3D& other)
  {
    v_[0] = other[0];
    v_[1] = other[1];
    v_[2] = other[2];
    v_[3] = 1;
  }

  Point4D& operator =(const Point4D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
    return *this;
  }

  double& operator[](size_t idx)
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const
  {
    return v_[ idx ];
  }

private:
  double v_[4];
};


class Vector3D
{
public:
  Vector3D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
  }
  Vector3D(Point3D p)
  {
    v_[0] = p[0];
    v_[1] = p[1];
    v_[2] = p[2];
  }
  Vector3D(double x, double y, double z)
  {
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
  }
  Vector3D(const Vector3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
  }

  Vector3D& operator =(const Vector3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

  double dot(const Vector3D& other) const
  {
    return v_[0]*other.v_[0] + v_[1]*other.v_[1] + v_[2]*other.v_[2];
  }

  double length2() const
  {
    return v_[0]*v_[0] + v_[1]*v_[1] + v_[2]*v_[2];
  }
  double length() const
  {
	  return sqrt(length2());
  }

  double get_x() const
  {
	  return v_[0];
  }

  double get_y() const
  {
	  return v_[1];
  }

  double get_z() const
  {
	  return v_[2];
  }

  double normalize();

  Vector3D cross(const Vector3D& other) const
  {
    return Vector3D(
                    v_[1]*other[2] - v_[2]*other[1],
                    v_[2]*other[0] - v_[0]*other[2],
                    v_[0]*other[1] - v_[1]*other[0]);
  }

  double coord_sum() const
  {
	  return (v_[0] + v_[1] + v_[2]);
  }

private:
  double v_[3];
};

inline Vector3D operator *(double s, const Vector3D& v)
{
  return Vector3D(s*v[0], s*v[1], s*v[2]);
}

inline Point3D operator *(double s, const Point3D& v)
{
  return Point3D(s*v[0], s*v[1], s*v[2]);
}

inline Vector3D operator +(const Vector3D& a, const Vector3D& b)
{
  return Vector3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline Point3D operator +(const Point3D& a, const Vector3D& b)
{
  return Point3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline Vector3D operator -(const Point3D& a, const Point3D& b)
{
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D operator -(const Vector3D& a, const Vector3D& b)
{
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D operator -(const Vector3D& a)
{
  return Vector3D(-a[0], -a[1], -a[2]);
}

inline Point3D operator -(const Point3D& a, const Vector3D& b)
{
  return Point3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D cross(const Vector3D& a, const Vector3D& b) 
{
  return a.cross(b);
}

inline std::ostream& operator <<(std::ostream& os, const Point2D& p)
{
  return os << "p<" << p[0] << "," << p[1] << ">";
}

inline std::ostream& operator <<(std::ostream& os, const Point3D& p)
{
  return os << "p<" << p[0] << "," << p[1] << "," << p[2] << ">";
}

inline std::ostream& operator <<(std::ostream& os, const Vector3D& v)
{
  return os << "v<" << v[0] << "," << v[1] << "," << v[2] << ">";
}

class Matrix4x4;

class Vector4D
{
public:
  Vector4D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
    v_[3] = 0.0;
  }
  Vector4D(double x, double y, double z, double w)
  { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
    v_[3] = w;
  }
  Vector4D(const Vector4D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
  }

  Vector4D& operator =(const Vector4D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

private:
  double v_[4];
};

class Matrix4x4
{
public:
  Matrix4x4()
  {
    // Construct an identity matrix
    std::fill(v_, v_+16, 0.0);
    v_[0] = 1.0;
    v_[5] = 1.0;
    v_[10] = 1.0;
    v_[15] = 1.0;
  }
  Matrix4x4(const Matrix4x4& other)
  {
    std::copy(other.v_, other.v_+16, v_);
  }
  Matrix4x4(const Vector4D row1, const Vector4D row2, const Vector4D row3, 
             const Vector4D row4)
  {
    v_[0] = row1[0]; 
    v_[1] = row1[1]; 
    v_[2] = row1[2]; 
    v_[3] = row1[3]; 

    v_[4] = row2[0]; 
    v_[5] = row2[1]; 
    v_[6] = row2[2]; 
    v_[7] = row2[3]; 

    v_[8] = row3[0]; 
    v_[9] = row3[1]; 
    v_[10] = row3[2]; 
    v_[11] = row3[3]; 

    v_[12] = row4[0]; 
    v_[13] = row4[1]; 
    v_[14] = row4[2]; 
    v_[15] = row4[3]; 
  }
  Matrix4x4(double *vals)
  {
    std::copy(vals, vals + 16, (double*)v_);
  }

  Matrix4x4& operator=(const Matrix4x4& other)
  {
    std::copy(other.v_, other.v_+16, v_);
    return *this;
  }

  Vector4D getRow(size_t row) const
  {
    return Vector4D(v_[4*row], v_[4*row+1], v_[4*row+2], v_[4*row+3]);
  }
  double *getRow(size_t row) 
  {
    return (double*)v_ + 4*row;
  }

  Vector4D getColumn(size_t col) const
  {
    return Vector4D(v_[col], v_[4+col], v_[8+col], v_[12+col]);
  }

  Vector4D operator[](size_t row) const
  {
    return getRow(row);
  }
  double *operator[](size_t row) 
  {
    return getRow(row);
  }

  Matrix4x4 transpose() const
  {
    return Matrix4x4(getColumn(0), getColumn(1), 
                      getColumn(2), getColumn(3));
  }
  Matrix4x4 invert() const;

  const double *begin() const
  {
    return (double*)v_;
  }
  const double *end() const
  {
    return begin() + 16;
  }

  // Return a matrix to represent a counterclockwise rotation of "angle"
  // degrees around the axis "axis", where "axis" is one of the
  // characters 'x', 'y', or 'z'.
    static Matrix4x4 rotation(double angle, char axis)
    {
        Matrix4x4 r;
        switch (axis)
        {
          case 'x':
            r[1][1] = cos(angle);
            r[1][2] = -sin(angle);
            r[2][1] = sin(angle);
            r[2][2] = cos(angle);
            break;
          case 'y':
            r[0][0] = cos(angle);
            r[0][2] = -sin(angle);
            r[2][0] = sin(angle);
            r[2][2] = cos(angle);
            break;
          case 'z':
            r[0][0] = cos(angle);
            r[0][1] = -sin(angle);
            r[1][0] = sin(angle);
            r[1][1] = cos(angle);
            break;
        }

        return r;
    }

    // Return a matrix to represent a displacement of the given vector.
    static Matrix4x4 translation(const Vector3D& displacement)
    {
        Matrix4x4 t;
        t[0][3] = displacement[0];
        t[1][3] = displacement[1];
        t[2][3] = displacement[2];

        return t;
    }

    // Return a matrix to represent a nonuniform scale with the given factors.
    static Matrix4x4 scaling(const Vector3D& scale)
    {
        Matrix4x4 s;
        s[0][0] = scale[0];
        s[1][1] = scale[1];
        s[2][2] = scale[2];
        s[3][3] = 1;

        return s;
    }

private:
  double v_[16];
};

inline Matrix4x4 operator *(const Matrix4x4& a, const Matrix4x4& b)
{
  Matrix4x4 ret;

  for(size_t i = 0; i < 4; ++i) {
    Vector4D row = a.getRow(i);
		
    for(size_t j = 0; j < 4; ++j) {
      ret[i][j] = row[0] * b[0][j] + row[1] * b[1][j] + 
        row[2] * b[2][j] + row[3] * b[3][j];
    }
  }

  return ret;
}

inline Vector3D operator *(const Matrix4x4& M, const Vector3D& v)
{
  return Vector3D(
                  v[0] * M[0][0] + v[1] * M[0][1] + v[2] * M[0][2],
                  v[0] * M[1][0] + v[1] * M[1][1] + v[2] * M[1][2],
                  v[0] * M[2][0] + v[1] * M[2][1] + v[2] * M[2][2]);
}

inline Point3D operator *(const Matrix4x4& M, const Point3D& p)
{
  return Point3D(
                 p[0] * M[0][0] + p[1] * M[0][1] + p[2] * M[0][2] + M[0][3],
                 p[0] * M[1][0] + p[1] * M[1][1] + p[2] * M[1][2] + M[1][3],
                 p[0] * M[2][0] + p[1] * M[2][1] + p[2] * M[2][2] + M[2][3]);
}

inline Point4D operator *(const Matrix4x4& M, const Point4D& p)
{
  return Point4D(
                 p[0] * M[0][0] + p[1] * M[0][1] + p[2] * M[0][2] + M[0][3],
                 p[0] * M[1][0] + p[1] * M[1][1] + p[2] * M[1][2] + M[1][3],
                 p[0] * M[2][0] + p[1] * M[2][1] + p[2] * M[2][2] + M[2][3],
                 p[0] * M[3][0] + p[1] * M[3][1] + p[2] * M[3][2] + M[3][3]);
}

inline Vector3D transNorm(const Matrix4x4& M, const Vector3D& n)
{
  return Vector3D(
                  n[0] * M[0][0] + n[1] * M[1][0] + n[2] * M[2][0],
                  n[0] * M[0][1] + n[1] * M[1][1] + n[2] * M[2][1],
                  n[0] * M[0][2] + n[1] * M[1][2] + n[2] * M[2][2]);
}

inline std::ostream& operator <<(std::ostream& os, const Matrix4x4& M)
{
  return os << "[" << M[0][0] << " " << M[0][1] << " " 
            << M[0][2] << " " << M[0][3] << "]" << std::endl
            << "[" << M[1][0] << " " << M[1][1] << " " 
            << M[1][2] << " " << M[1][3] << "]" << std::endl
            << "[" << M[2][0] << " " << M[2][1] << " " 
            << M[2][2] << " " << M[2][3] << "]" << std::endl
            << "[" << M[3][0] << " " << M[3][1] << " " 
            << M[3][2] << " " << M[3][3] << "]";
}

class Color
{
public:
    Color()
    {
        r_ = 0;
        g_ = 0;
        b_ = 0;
    }
    Color(double r, double g, double b)
      : r_(r)
      , g_(g)
      , b_(b)
    {}
  Color(double c)
    : r_(c)
    , g_(c)
    , b_(c)
  {}
  Color(const Color& other)
    : r_(other.r_)
    , g_(other.g_)
    , b_(other.b_)
  {}

  Color& operator =(const Color& other)
  {
    r_ = other.r_;
    g_ = other.g_;
    b_ = other.b_;
    return *this;
  }

  double R() const 
  { 
    return r_;
  }
  double G() const 
  { 
    return g_;
  }
  double B() const 
  { 
    return b_;
  }
private:
  double r_;
  double g_;
  double b_;
};

inline Color operator *(double s, const Color& a)
{
  return Color(s*a.R(), s*a.G(), s*a.B());
}

inline Color operator *(const Color& a, const Color& b)
{
  return Color(a.R()*b.R(), a.G()*b.G(), a.B()*b.B());
}

inline Color operator /(const Color& a, float b)
{
  return Color(a.R()/b, a.G()/b, a.B()/b);
}

inline Color operator +(const Color& a, const Color& b)
{
  return Color(a.R()+b.R(), a.G()+b.G(), a.B()+b.B());
}

inline std::ostream& operator <<(std::ostream& os, const Color& c)
{
  return os << "c<" << c.R() << "," << c.G() << "," << c.B() << ">";
}

class Line3D
{
public:
    Line3D();
    ~Line3D();
    Line3D(const Point3D &p1, Point3D &p2);
    Line3D(const Line3D &other);
    Point3D &getP1();
    Point3D &getP2();
private:
    Point3D p1_, p2_;
};

enum ShapeType {CUBE, PYRAMID, CYLINDER};

class Shape
{
public:
    Shape(ShapeType sType);
    ~Shape();

    Matrix4x4 getTransform() const;         // return transform_ * scale_
    Matrix4x4 getGnomonTransform() const;   // return only transform_
    void resetTransform();
    void appendTransform(const Matrix4x4 &xform);   // append to transform_
    void scale(const Matrix4x4 &xform);     // append to scale_

    std::vector<Line3D> getLines();

private:
    std::vector<Point3D> verts_;
    Matrix4x4 scale_;
    Matrix4x4 transform_;
};

#endif
