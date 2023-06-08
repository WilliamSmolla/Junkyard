#pragma once
#include <random>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <memory>

static std::random_device device;
static std::mt19937_64 random_engine(device());
static std::uniform_int_distribution<uint64_t> uniform_distribution;

const uint32_t max_entities = 1000;
const uint8_t max_components = 255;

static int component_counter = 0;

template <class T>
static int get_component_id() {
	static int component_id = component_counter++;
	return component_id;
}

struct entity_desc {
	uint64_t id;
	std::bitset<max_components> mask;
};

struct component_pool {
	component_pool(size_t size)
		: _size{ size } {
		_data = new char[size * max_entities];
	}

	~component_pool() {
		delete[] _data;
	}

	void remove_data(uint64_t entity) {
		size_t entityIndex = _entity_to_index_map[entity];
		size_t lastIndex = _nb_of_components - 1;

		_data[entityIndex] = _data[lastIndex];

		_entity_to_index_map[lastIndex] = entityIndex;
		_index_to_entity_map[entityIndex] = lastIndex;

		_entity_to_index_map.erase(entity);
		_index_to_entity_map.erase(lastIndex);

		--_nb_of_components;
	}

	void* set_data(uint64_t entity) {
		size_t newIndex = _nb_of_components;

		_entity_to_index_map[entity] = newIndex;
		_index_to_entity_map[newIndex] = entity;

		++_nb_of_components;

		return _data + _entity_to_index_map[entity] * _size;
	}

	inline void* get_data(uint64_t entity) {
		return _data + _entity_to_index_map[entity] * _size;
	}

private:
	char* _data;
	size_t _size;
	unsigned int _nb_of_components = 0;

	std::unordered_map<uint64_t, size_t> _entity_to_index_map;
	std::unordered_map<size_t, uint64_t> _index_to_entity_map;
};

class entity_registry {
public:
	entity_registry() = default;
	~entity_registry() {
		clear();
	}

	entity_desc& create_entity() {
		_entities.push_back({ uniform_distribution(random_engine), std::bitset<max_components>() });
		return _entities.back();
	}

	entity_desc& create_entity_from_id(uint64_t id) {
		_entities.push_back({ id, std::bitset<max_components>() });
		return _entities.back();
	}

	void destroy_entity(uint64_t entity) {
		auto top_it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (top_it != _entities.end()) {
			for (auto& component_pool : _component_pools) {
				component_pool->remove_data(entity);
			}

			_entities.erase(top_it);

			for (auto& view : _view_any_of_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					view.second.erase(it);
				}
			}

			for (auto& view : _view_all_of_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					view.second.erase(it);
				}
			}

			for (auto& view : _view_exlusively_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					view.second.erase(it);
				}
			}

			for (auto& group : _entity_groups) {
				remove_entity_from_group(entity, group.first);
			}

			return;
		}
	}

	template<typename T>
	T* assign_component(uint64_t entity) {
		int component_id = get_component_id<T>();

		if (_component_pools.size() <= component_id) {
			_component_pools.resize(component_id + 1, nullptr);
		}

		if (_component_pools[component_id] == nullptr) {
			_component_pools[component_id] = std::make_shared<component_pool>(sizeof(T));
		}

		auto top_it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (top_it != _entities.end()) {
			auto top_index = std::distance(_entities.begin(), top_it);

			T* component = new (_component_pools[component_id]->set_data(entity))T();

			_entities[top_index].mask.set(component_id);

			for (auto& view : _view_any_of_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					auto any_index = std::distance(view.second.begin(), it);

					view.second[any_index].mask = _entities.at(top_index).mask;
				}
			}

			for (auto& view : _view_all_of_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					auto all_index = std::distance(view.second.begin(), it);

					view.second.at(all_index).mask = _entities.at(top_index).mask;
				}
			}

			for (auto& view : _view_exlusively_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					auto ex_index = std::distance(view.second.begin(), it);

					view.second.at(ex_index).mask = _entities.at(top_index).mask;

					if (view.first != view.second.at(ex_index).mask) {
						view.second.erase(it);
					}
				}
			}

			for (auto& group : _entity_groups) {
				auto it = std::find_if(group.second.begin(), group.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != group.second.end()) {
					auto grp_index = std::distance(group.second.begin(), it);
					group.second.at(grp_index).mask = _entities.at(top_index).mask;
				}
			}

			return component;
		}

		return nullptr;
	}

	template<typename T>
	void remove_component(uint64_t entity) {
		int component_id = get_component_id<T>();

		if (_component_pools.size() <= component_id) {
			_component_pools.resize(component_id + 1, nullptr);
		}

		if (_component_pools[component_id] == nullptr) {
			_component_pools[component_id] = std::make_shared<component_pool>(sizeof(T));
		}

		auto top_it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (top_it != _entities.end()) {
			_component_pools[component_id]->remove_data(entity);
			_entities.at(top_it).mask.reset(component_id);

			for (auto& view : _view_any_of_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					view.second.at(it).mask = _entities.at(top_it).mask;

					bool to_delete = false;

					for (size_t i = 0; i < max_components; i++) {
						if (view.second.at(it).mask[i] == view.first[i]) {
							to_delete = false;
							break;
						}

						to_delete = true;
					}

					if (to_delete) { view.second.erase(it); }
				}
			}

			for (auto& view : _view_all_of_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					view.second.at(it).mask = _entities.at(top_it).mask;

					if ((view.first & ~(view.second.at(it).mask)).none()) {
						view.second.erase(it);
					}
				}
			}

			for (auto& view : _view_exlusively_map) {
				auto it = std::find_if(view.second.begin(), view.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != view.second.end()) {
					view.second.at(it).mask = _entities.at(top_it).mask;

					if (view.first != view.second.at(it).mask) {
						view.second.erase(it);
						return;
					}
				}
			}

			for (auto& group : _entity_groups) {
				auto it = std::find_if(group.second.begin(), group.second.end(), [&entity](const entity_desc& desc) {
					return desc.id == entity;
					});

				if (it != group.second.end()) {
					group.second.at(it).mask = _entities.at(top_it).mask;
				}
			}
		}
	}

	template<typename T>
	T* get_component(uint64_t entity) {
		auto it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (it != _entities.end()) {
			// todo: check if entity already have this component
			int component_id = get_component_id<T>();
			auto index = std::distance(_entities.begin(), it);

			if (!_entities.at(index).mask.test(component_id))
			{
				// does not have this component...log this.
			}
			auto component = static_cast<T*>(_component_pools[component_id]->get_data(entity));
			return component;
		}

		return nullptr;
	}

	template<typename T>
	bool has_component(uint64_t entity) {
		auto it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (it != _entities.end()) {
			int component_id = get_component_id<T>();
			auto index = std::distance(_entities.begin(), it);

			if (!_entities.at(index).mask.test(component_id)) {
				return false;
			}

			return true;
		}

		return false;
	}

	void add_entity_to_group(uint64_t entity, const std::string& group_name) {
		if (!_entity_groups.contains(group_name)) {
			_entity_groups[group_name] = std::vector<entity_desc>{};
		}

		auto it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (it != _entities.end()) {
			auto index = std::distance(_entities.begin(), it);

			_entity_groups.at(group_name).push_back(_entities[index]);
		}
	}

	void remove_entity_from_group(uint64_t entity, const std::string& group_name) {
		if (!_entity_groups.contains(group_name)) {
			return;
		}

		auto it = std::find_if(_entity_groups.at(group_name).begin(), _entity_groups.at(group_name).end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (it != _entities.end()) {
			_entity_groups.at(group_name).erase(it);
		}
	}

	std::vector<entity_desc> get_entity_group(const std::string& group_name) const {
		if (!_entity_groups.contains(group_name)) {
			std::vector<entity_desc> empty;
			return empty;
		}
		return _entity_groups.at(group_name);
	}

	std::vector<entity_desc> get_entities() const {
		return _entities;
	}

	size_t entity_to_index(uint64_t entity) const {
		auto it = std::find_if(_entities.begin(), _entities.end(), [&entity](const entity_desc& desc) {
			return desc.id == entity;
			});

		if (it != _entities.end()) {
			auto index = std::distance(_entities.begin(), it);
			return index;
		}
		return -1;
	}

	uint64_t index_to_entity(size_t index) {
		if (index > _entities.size() || index < 0) {
			return 0;
		}
		return _entities[index].id;
	}

	template<typename... component_types>
	std::vector<entity_desc>& view_exclusively() {
		std::vector<int> component_ids = { 0, get_component_id<component_types>() ... };
		std::bitset<max_components> view_mask;

		for (size_t i = 1; i < sizeof...(component_types) + 1; i++) { view_mask.set(i); }

		if (sizeof...(component_types) == 0) {
			return _entities;
		}
		else if (_view_exlusively_map.find(view_mask) != _view_exlusively_map.end()) {
			return _view_exlusively_map[view_mask];
		}
		else {
			std::vector<entity_desc> view;

			for (auto entity : _entities)
			{
				if (view_mask == entity.mask)
				{
					view.push_back(entity);
				}
			}

			_view_exlusively_map[view_mask] = view;

			return _view_exlusively_map[view_mask];
		}

		std::vector<entity_desc> empty_vec{};
		return empty_vec;
	}

	template<typename... component_types>
	std::vector<entity_desc>& view_all_of() {
		std::vector<int> component_ids = { 0, get_component_id<component_types>() ... };
		std::bitset<max_components> view_mask;

		for (size_t i = 1; i < sizeof...(component_types) + 1; i++) { view_mask.set(i); }

		if (sizeof...(component_types) == 0) {
			return _entities;
		}
		else if (_view_all_of_map.find(view_mask) != _view_all_of_map.end()) {
			return _view_all_of_map[view_mask];
		}
		else {
			std::vector<entity_desc> view;

			for (auto& entity : _entities) {
				unsigned int result = 0;

				for (size_t i; i < sizeof...(component_types) + 1; i++) {
					if (entity.mask.test(component_ids[i])) {
						result++;
					}
				}

				if (result == component_ids.size() - 1) {
					view.push_back(entity);
				}
			}

			_view_all_of_map[view_mask] = view;

			return _view_all_of_map[view_mask];
		}
		std::vector<entity_desc> empty_vec{};
		return empty_vec; nullptr;
	}

	template<typename... component_types>
	std::vector<entity_desc>& view_any_of() {
		std::vector<int> component_ids = { 0, get_component_id<component_types>() ... };
		std::bitset<max_components> view_mask;

		for (size_t i = 1; i < sizeof...(component_types) + 1; i++) { view_mask.set(i); }

		if (sizeof...(component_types) == 0) {
			return _entities;
		}
		else if (_view_any_of_map.find(view_mask) != _view_any_of_map.end()) {
			return _view_any_of_map[view_mask];
		}
		else {
			std::vector<entity_desc> view;

			for (auto entity : _entities) {
				for (size_t i = 1; i < sizeof...(component_types) + 1; i++) {
					if (entity.mask.test(component_ids[i])) {
						view.push_back(entity);
						break;
					}
				}
			}

			_view_any_of_map[view_mask] = view;

			return _view_any_of_map[view_mask];
		}

		std::vector<entity_desc> empty_vec{};
		return empty_vec;
	}

	// Is that really necessary ? I mean, everything get destroyed at the end of the scope anyway.
	void clear() {
		_entities.clear();
		_component_pools.clear();
		_view_any_of_map.clear();
		_view_exlusively_map.clear();
		_entity_groups.clear();
	}

private:
	std::vector<entity_desc> _entities;
	std::vector<std::shared_ptr<component_pool>> _component_pools;
	std::unordered_map<std::bitset<max_components>, std::vector<entity_desc>> _view_any_of_map;
	std::unordered_map<std::bitset<max_components>, std::vector<entity_desc>> _view_all_of_map;
	std::unordered_map<std::bitset<max_components>, std::vector<entity_desc>> _view_exlusively_map;
	std::unordered_map<std::string, std::vector<entity_desc>> _entity_groups;
};
