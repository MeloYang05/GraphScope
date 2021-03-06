/** Copyright 2020 Alibaba Group Holding Limited.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef ANALYTICAL_ENGINE_APPS_BOUNDARY_EDGE_BOUNDARY_H_
#define ANALYTICAL_ENGINE_APPS_BOUNDARY_EDGE_BOUNDARY_H_

#include <set>
#include <utility>
#include <vector>

#include "grape/grape.h"

#include "apps/boundary/edge_boundary_context.h"
#include "core/app/app_base.h"

namespace gs {
/**
 * @brief Compute the edge boundary for given vertices.
 * @tparam FRAG_T
 */
template <typename FRAG_T>
class EdgeBoundary : public AppBase<FRAG_T, EdgeBoundaryContext<FRAG_T>>,
                     public grape::Communicator {
 public:
  INSTALL_DEFAULT_WORKER(EdgeBoundary<FRAG_T>, EdgeBoundaryContext<FRAG_T>,
                         FRAG_T)
  using oid_t = typename fragment_t::oid_t;
  using vid_t = typename fragment_t::vid_t;
  using vertex_t = typename fragment_t::vertex_t;

  static constexpr grape::LoadStrategy load_strategy =
      grape::LoadStrategy::kBothOutIn;

  void PEval(const fragment_t& frag, context_t& ctx,
             message_manager_t& messages) {
    // parse input node array from json
    folly::dynamic node_array_1 = folly::parseJson(ctx.nbunch1);
    std::set<vid_t> node_gid_set, node_gid_set_2;
    vid_t gid;
    vertex_t u;
    for (const auto& oid : node_array_1) {
      if (frag.Oid2Gid(oid, gid)) {
        node_gid_set.insert(gid);
      }
    }
    if (!ctx.nbunch2.empty()) {
      auto node_array_2 = folly::parseJson(ctx.nbunch2);
      for (const auto& oid : node_array_2) {
        if (frag.Oid2Gid(oid, gid)) {
          node_gid_set_2.insert(gid);
        }
      }
    }

    // get the boundary
    for (auto& gid : node_gid_set) {
      if (frag.InnerVertexGid2Vertex(gid, u)) {
        for (auto e : frag.GetOutgoingAdjList(u)) {
          vid_t vgid = frag.Vertex2Gid(e.get_neighbor());
          if (node_gid_set_2.empty()) {
            if (node_gid_set.find(vgid) == node_gid_set.end()) {
              ctx.boundary.insert(std::make_pair(gid, vgid));
            }
          } else {
            if (node_gid_set_2.find(vgid) != node_gid_set_2.end()) {
              ctx.boundary.insert(std::make_pair(gid, vgid));
            }
          }
        }
      }
    }

    // gather and process boundary on worker-0
    std::vector<std::set<std::pair<vid_t, vid_t>>> all_boundary;
    AllGather(ctx.boundary, all_boundary);

    if (frag.fid() == 0) {
      for (size_t i = 1; i < all_boundary.size(); ++i) {
        for (auto& v : all_boundary[i]) {
          ctx.boundary.insert(v);
        }
      }
      writeToCtx(frag, ctx);
    }
  }

  void IncEval(const fragment_t& frag, context_t& ctx,
               message_manager_t& messages) {
    // Yes, there's no any code in IncEval.
    // Refer:
    // https://networkx.org/documentation/stable/_modules/networkx/algorithms/boundary.html#edge_boundary
  }

 private:
  void writeToCtx(const fragment_t& frag, context_t& ctx) {
    std::vector<typename fragment_t::oid_t> data;
    for (auto& e : ctx.boundary) {
      data.push_back(frag.Gid2Oid(e.first));
      data.push_back(frag.Gid2Oid(e.second));
    }
    std::vector<size_t> shape{data.size() / 2, 2};
    ctx.assign(data, shape);
  }
};
}  // namespace gs

#endif  // ANALYTICAL_ENGINE_APPS_BOUNDARY_EDGE_BOUNDARY_H_
