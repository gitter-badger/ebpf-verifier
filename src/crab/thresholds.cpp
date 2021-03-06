#include "crab/thresholds.hpp"
#include "crab/cfg.hpp"

namespace crab {

inline namespace iterators {

void thresholds_t::add(bound_t v1) {
    if (m_thresholds.size() < m_size) {
        bound_t v = (v1);
        if (std::find(m_thresholds.begin(), m_thresholds.end(), v) == m_thresholds.end()) {
            auto ub = std::upper_bound(m_thresholds.begin(), m_thresholds.end(), v);

            // don't add consecutive thresholds
            if (v > 0) {
                auto prev = ub;
                --prev;
                if (prev != m_thresholds.begin()) {
                    if (*prev + 1 == v) {
                        *prev = v;
                        return;
                    }
                }
            } else if (v < 0) {
                if (*ub - 1 == v) {
                    *ub = v;
                    return;
                }
            }

            m_thresholds.insert(ub, v);
        }
    }
}

std::ostream& operator<<(std::ostream& o, const thresholds_t& t) {
    o << "{";
    for (typename std::vector<bound_t>::const_iterator it = t.m_thresholds.begin(), et = t.m_thresholds.end(); it != et;) {
        bound_t b(*it);
        o << b;
        ++it;
        if (it != t.m_thresholds.end())
            o << ",";
    }
    o << "}";
    return o;
}

void wto_thresholds_t::get_thresholds(const basic_block_t& bb, thresholds_t& thresholds) const {

}

void wto_thresholds_t::operator()(wto_vertex_t& vertex) {
    if (m_stack.empty())
        return;

    label_t head = m_stack.back();
    auto it = m_head_to_thresholds.find(head);
    if (it != m_head_to_thresholds.end()) {
        thresholds_t& thresholds = it->second;
        auto& bb = m_cfg.get_node(vertex.node());
        get_thresholds(bb, thresholds);
    } else {
        CRAB_ERROR("No head found while gathering thresholds");
    }
}

void wto_thresholds_t::operator()(wto_cycle_t& cycle) {
    thresholds_t thresholds(m_max_size);
    auto& bb = m_cfg.get_node(cycle.head());
    get_thresholds(bb, thresholds);

    // XXX: if we want to consider constants from loop
    // initializations
    for (auto pre : boost::make_iterator_range(bb.prev_blocks())) {
        if (pre != cycle.head()) {
            auto& pred_bb = m_cfg.get_node(pre);
            get_thresholds(pred_bb, thresholds);
        }
    }

    m_head_to_thresholds.insert(std::make_pair(cycle.head(), thresholds));
    m_stack.push_back(cycle.head());
    for (wto_component_t& c : cycle) {
        std::visit(*this, c);
    }
    m_stack.pop_back();
}

std::ostream& operator<<(std::ostream& o, const wto_thresholds_t& t) {
    for (auto& [label, th] : t.m_head_to_thresholds) {
        o << label << "=" << th << "\n";
    }
    return o;
}
} // namespace iterators

} // namespace crab