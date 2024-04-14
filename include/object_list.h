#include "object.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class Object_list : public Object {
public:
    std::vector<shared_ptr<Object>> objects;

    Object_list() {}
    Object_list(shared_ptr<Object> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Object> object) {
        objects.push_back(object);
    }

    bool hit(Ray& r, Interval ray_t, Hit_record& rec) const override {
        Hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};