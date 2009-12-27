/*
  This file is part of MADNESS.
  
  Copyright (C) 2007,2010 Oak Ridge National Laboratory
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  
  For more information please contact:
  
  Robert J. Harrison
  Oak Ridge National Laboratory
  One Bethel Valley Road
  P.O. Box 2008, MS-6367
  
  email: harrisonrj@ornl.gov
  tel:   865-241-3937
  fax:   865-572-0680
  
  $Id$
*/
#define WORLD_INSTANTIATE_STATIC_TEMPLATES
#include <world/world.h>

using namespace madness;
using namespace std;


struct Key {
    int k;

    Key() : k(-1) {}

    Key(int k) : k(k) {}

    hashT hash() const {
        return k;
    }

    template <typename Archive>
    void serialize(const Archive& ar) {
        ar & k;
    }

    bool operator==(const Key& b) const {
        return k==b.k;
    }
};

ostream& operator<<(ostream&s, const Key& key) {
    s << "Key(" << key.k << ")";
    return s;
}

struct Node {
    int k;

    Node() : k(-1) {}

    Node(int k) : k(k) {}

    int get() const {
        return k;
    }

    template <typename Archive>
    void serialize(const Archive& ar) {
        ar & k;
    }
};

ostream& operator<<(ostream&s, const Node& node) {
    s << "Node(" << node.k << ")";
    return s;
}


void test0(World& world) {
    WorldContainer<Key,Node> c(world);

    Key key1(1);
    Node node1(1);

    if (c.owner(key1) == world.rank()) c.replace(key1,node1);

    world.gop.fence();

    for (int i=0; i<10000; i++)
        MADNESS_ASSERT(c.find(key1).get()->second.get() == 1);

    for (int i=3; i<100; i++)
        MADNESS_ASSERT(c.find(Key(i)).get() == c.end());


    world.gop.fence();
}



int main(int argc, char** argv) {
    bool bind[3] = {true, true, true};
    int cpulo[3] = {0, 1, 2};
    ThreadBase::set_affinity_pattern(bind, cpulo); // Decide how to locate threads before doing anything
    ThreadBase::set_affinity(0);         // The main thread is logical thread 0
    MPI::Init(argc, argv);      // MPI starts the universe
    ThreadPool::begin();        // Must have thread pool before any AM arrives
    RMI::begin();               // Must have RMI while still running single threaded

    World world(MPI::COMM_WORLD);
    redirectio(world);
    world.gop.fence();

    xterm_debug("./testdc", 0);

    try {
        test0(world);
    }
    catch (MPI::Exception e) {
        error("caught an MPI exception");
    }
    catch (madness::MadnessException e) {
        print(e);
        error("caught a MADNESS exception");
    }
    catch (const char* s) {
        print(s);
        error("caught a string exception");
    }
    catch (...) {
        error("caught unhandled exception");
    }

    world.gop.fence();
    RMI::end();
    MPI::Finalize();
    return 0;
}
