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

#ifndef QUADDISTTREE_H
#define QUADDISTTREE_H

#include "utils.h"
#include "hashutils.h"
#include "dtree.h"
#include "multilinear.h"
#include "intersector.h"
#include "pointprojector.h"
#include <numeric>
#include <map>
#include <string.h>

template <int Dim>
class DistFunction : public Multilinear<double, Dim>
{
    typedef Multilinear<double, Dim> super;
    typedef Rect<double, Dim> MyRect;

public:
    template <class Eval>
    void initFunc(const Eval &eval, const MyRect &rect)
    {
        for (int i = 0; i < (1 << Dim); ++i)
        {
            this->setValue(i, eval(rect.getCorner(i)));
        }
        return;
    }

private:
};

template <int Dim>
class DistData : public DistFunction<Dim>
{
public:
    typedef DistFunction<Dim> super;
    typedef DNode<DistData<Dim>, Dim> NodeType;

    DistData(NodeType *inNode) : node(inNode) {}

    void init() {}

    template <class Eval, template <typename Node, int IDim> class Indexer>
    void fullSplit(const Eval &eval, double tol, DRootNode<DistData<Dim>, Dim, Indexer> *rootNode, int level = 0, bool cropOutside = false)
    {
        int i;
        // for each corner of the node's rect
        // calcuate rect_corner's distance to surface
        const Rect<double, Dim> &rect = node->getRect();
        node->initFunc(eval, rect);

        // check whether the node's rect is inside or outside the surface
        // by comparing `rect's diagnoal length` and `the distance of rect's center to surface`
        // if the entire rect is outside the surface, just return
        bool nextCropOutside = cropOutside;
        if (cropOutside && level > 0)
        {
            double distCentToSurf = eval(rect.getCenter());
            double len = rect.getSize().length() * 0.5;
            if (distCentToSurf > len)
                return;
            if (distCentToSurf < -len)
                nextCropOutside = false;
        }

        // part of the rect is inside the surface, try to split
        // condition1: exceed the maximum depth, do not split
        if (level == (32 / Dim))
            return;

        // condition2: root node, always split
        bool doSplit = false;
        if (node->getParent() == NULL)
            doSplit = true;

        // condition3: child node
        // subvided when the distance field within a cell is not well
        // approximated by multilinear interpolation of its corner values
        if (!doSplit)
        {
            // if Dim=2: permute the 3x3 cases, [low,low], [mid, low], ... , [hi, hi]
            // if Dim=3: permute the 3x3x3 cases, [low,low,low], [mid, low, low],...,[hi, hi, hi]
            Vector<double, Dim> center = rect.getCenter();
            const Vector<double, Dim> &lo = rect.getLo();
            const Vector<double, Dim> &hi = rect.getHi();
            for (int i = 0; i < grid.num_corners; ++i) // the i_th corner
            {
                Vector<double, Dim> cur;
                bool anyMid = false;
                // populate cur according to corner_bits, low, center, corner of rect
                {
                    for (int k = 0; k < Dim; ++k)
                    {
                        int bit = grid.bit(i, k);
                        cur[k] = (bit == 0) ? lo[k] : (bit == 1 ? hi[k] : center[k]);
                        if (bit == 2)
                        {
                            anyMid = true;
                        }
                    }
                }

                // evaluate(cur): approximated distance field
                // eval(cur): exact distance to surface
                if (anyMid && fabs(evaluate(cur) - eval(cur)) > tol)
                {
                    doSplit = true;
                    break;
                }
            }
        }

        if (!doSplit)
            return;

        rootNode->split(node);
        for (i = 0; i < NodeType::numChildren; ++i)
        {
            eval.setRect(Rect<double, Dim>(rect.getCorner(i)) | Rect<double, Dim>(rect.getCenter()));
            node->getChild(i)->fullSplit(eval, tol, rootNode, level + 1, nextCropOutside);
        }
    }

    template <class Real>
    Real evaluate(const Vector<Real, Dim> &v)
    {
        if (node->getChild(0) == NULL)
        {
            auto w = (v - node->getRect().getLo());
            auto rSize = node->getRect().getSize();
            w = w.apply(divides<Real>(), rSize);
            return super::evaluate((v - node->getRect().getLo()).apply(divides<Real>(), node->getRect().getSize()));
        }
        Vector<Real, Dim> center = node->getRect().getCenter();
        int idx = 0;
        for (int i = 0; i < Dim; ++i)
            if (v[i] > center[i])
                idx += (1 << i);
        return node->getChild(idx)->evaluate(v);
    }

    template <class Real>
    Real integrate(Rect<Real, Dim> r)
    {
        r &= Rect<Real, Dim>(node->getRect());
        if (r.isEmpty())
            return Real();
        if (node->getChild(0) == NULL)
        {
            Vector<Real, Dim> corner = node->getRect().getLo(), size = node->getRect().getSize();
            Rect<Real, Dim> adjRect((r.getLo() - corner).apply(divides<Real>(), size),
                                    (r.getHi() - corner).apply(divides<Real>(), size));
            return Real(node->getRect().getContent()) * super::integrate(adjRect);
        }
        Real out = Real();
        for (int i = 0; i < NodeType::numChildren; ++i)
            out += node->getChild(i)->integrate(r);
        return out;
    }

private:
    NodeType *node;
    static const Grid3<Dim> grid;
};

template <int Dim>
const Grid3<Dim> DistData<Dim>::grid;

typedef DistData<3>::NodeType OctTreeNode;
typedef DRootNode<DistData<3>, 3> OctTreeRoot;

template <class RootNode = OctTreeRoot>
class OctTreeMaker
{
public:
    static RootNode *make(const ObjectProjector<3, Tri3Object> &proj, const Mesh &m, double tol)
    {
        DistObjEval eval(proj, m);
        RootNode *out = new RootNode();

        out->fullSplit(eval, tol, out, 0, true);
        out->preprocessIndex();

        return out;
    }

    static RootNode *make(const ObjectProjector<3, Vec3Object> &proj, double tol, const RootNode *dTree = NULL)
    {
        PointObjDistEval eval(proj, dTree);
        RootNode *out = new RootNode();

        out->fullSplit(eval, tol, out);
        out->preprocessIndex();

        return out;
    }

private:
    class DistObjEval
    {
    public:
        DistObjEval(const ObjectProjector<3, Tri3Object> &inProj, const Mesh &m) : proj(inProj), mint(m, Vector3(1, 0, 0))
        {
            level = 0;
            rects[0] = Rect3(Vector3(), Vector3(1.));
            inside[0] = 0;
        }

        double operator()(const Vector3 &vec) const
        {
            unsigned int cur = ROUND(vec[0] * 1023.) + 1024 * (ROUND(vec[1] * 1023.) + 1024 * ROUND(vec[2] * 1023.));
            unsigned int sz = cache.size();
            double &d = cache[cur];
            if (sz == cache.size())
                return d;
            return d = compute(vec);
        }

        void setRect(const Rect3 &r) const
        {
            while (!(rects[level].contains(r.getCenter())))
                --level;

            inside[level + 1] = inside[level];
            ++level;
            if (!inside[level])
            {
                double d = (*this)(r.getCenter());
                double diag2 = r.getSize().length() * 0.5;

                if (d >= diag2)
                    inside[level] = 1;
                else if (d <= -diag2)
                    inside[level] = -1;
            }

            rects[level] = r;
        }

    private:
        double compute(const Vector3 &vec) const
        {
            int i, ins = inside[level];
            if (!ins)
            {
                ins = 1;
                vector<Vector3> isecs = mint.intersect(vec);
                for (i = 0; i < (int)isecs.size(); ++i)
                {
                    // since the view direction is (1, 0, 0),
                    // all points are projected btw [0, 1]
                    // this method does not work for multi-components/non-manifold meshes!!!
                    // for a watertight connected non-manifold mesh, we can test wheter a point
                    // is inside the component or not through number of intersections
                    if (isecs[i][0] > vec[0])
                        ins = -ins;
                }
            }

            return (vec - proj.project(vec)).length() * ins;
        }

        mutable map<unsigned int, double> cache;
        const ObjectProjector<3, Tri3Object> &proj;
        Intersector mint; // this intersector is not precise, to construct a precise octree, we need a more precise intersector
        mutable Rect3 rects[11];
        mutable int inside[11];
        mutable int level; // essentially index of last rect
    };

    class PointObjDistEval
    {
    public:
        PointObjDistEval(const ObjectProjector<3, Vec3Object> &inProj, const RootNode *inDTree) : proj(inProj), dTree(inDTree) {}

        double operator()(const Vector3 &vec) const
        {
            unsigned int cur = ROUND(vec[0] * 1023.) + 1024 * (ROUND(vec[1] * 1023.) + 1024 * ROUND(vec[2] * 1023.));
            unsigned int sz = cache.size();
            double &d = cache[cur];
            if (sz == cache.size())
                return d;
            return d = (vec - proj.project(vec)).length();
        }

        void setRect(const Rect3 &r) const {}

    private:
        mutable hash_map<unsigned int, double> cache;
        const ObjectProjector<3, Vec3Object> &proj;
        const RootNode *dTree;
    };
};
#endif
