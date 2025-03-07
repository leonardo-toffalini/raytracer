#pragma once

#include "rt_common.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <strstream>

shared_ptr<hittable_list> read_obj(std::string filePath, shared_ptr<material> mat) {
  auto sides = make_shared<hittable_list>();

  std::vector<vec3> verts;
  std::ifstream f(filePath);
  if (!f.is_open())
    return sides;

  while (!f.eof()) {
    char line[128];
    f.getline(line, 128);

    std::strstream s;
    s << line;

    char temp;

    if (line[0] == 'v') {
      vec3 v;
      double x, y, z;
      s >> temp >> x >> y >> z;
      verts.push_back(vec3(x, y, z));
    }

    if (line[0] == 'f') {
      int face[3];
      s >> temp >> face[0] >> face[1] >> face[2];
      auto t = make_shared<tri>(verts[face[0] - 1], verts[face[1] - 1], verts[face[2] - 1], mat);
      sides->add(t);
    }
  }

  return sides;
}
