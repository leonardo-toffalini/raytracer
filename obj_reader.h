#pragma once

#include "rt_common.h"
#include <fstream>
#include <iostream>
#include <sstream>

shared_ptr<hittable_list> read_obj(std::string file_path, shared_ptr<material> mat) {
  auto sides = make_shared<hittable_list>();

  std::vector<vec3> verts;
  std::ifstream f = std::ifstream(file_path);
  if (!f.is_open()) {
    std::clog << "Unable to open " << file_path << std::endl;
    return sides;
  }

  while (!f.eof()) {
    char line[128];
    f.getline(line, 128);

    auto s = std::istringstream(line);

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
      std::vector<vec3> corners = {verts[face[0] - 1], verts[face[1] - 1], verts[face[2] - 1]};
      auto t = make_shared<tri>(corners[0], corners[1], corners[2], mat, 0);
      // std::clog << "(" << corners[0] << ") (" << corners[1] << ") (" << corners[2] << ")\n";
      sides->add(t);
    }
  }

  return sides;
}
