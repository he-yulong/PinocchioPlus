/*  This file is part of the Pinocchio automatic rigging library.
    Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MULTILINEAR_H_INCLUDED
#define MULTILINEAR_H_INCLUDED

#include "rect.h"

// multilinear function over unit hypercube
template <class Value, int Dim>
class Multilinear
{
public:
  Multilinear()
  {
    for (int i = 0; i < num; ++i)
      values[i] = Value();
  }

  void setValue(int idx, const Value &value) { values[idx] = value; }
  const Value &getValue(int idx) const { return values[idx]; }

  template <class Real>
  Real evaluate(const Vector<Real, Dim> &v) const
  {
    // bilinear, trilinear interpolation
    Real out(0);
    for (int i = 0; i < num; ++i)
    {
      Vector<Real, Dim> corner;
      BitComparator<Dim>::assignCorner(i, v, Vector<Real, Dim>(1.) - v, corner);
      Real factor = corner.accumulate(ident<Real>(), multiplies<Real>());
      out += (factor * Real(values[i]));
    }
    return out;
  }

  template <class Real>
  Real integrate(const Rect<Real, Dim> &r) const
  {
    return r.isEmpty() ? Real() : evaluate(r.getCenter()) * r.getContent();
  }

private:
  Multilinear(const Multilinear &); // noncopyable

  template <class Real>
  static Real pos(const Real &r1, const Real &r2)
  {
    if (r1 <= Real(0) && r2 <= Real(0))
      return Real(0);
    if (r1 >= Real(0) && r2 >= Real(0))
      return Real(1);
    if (r2 >= r1)
      return r2 / (r2 - r1);
    return r1 / (r1 - r2);
  }

  static const int num = (1 << Dim);
  Value values[num];
};

#endif // MULTILINEAR_H_INCLUDED
