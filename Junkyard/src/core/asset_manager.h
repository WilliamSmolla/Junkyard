#pragma once
#include "assets.h"

class asset_manager {
public:
	static asset_manager& get_instance() {
		static asset_manager instance;
		return instance;
	}

	asset_manager(const asset_manager&) = delete;
	void operator=(const asset_manager) = delete;

	static void init() { get_instance()._init(); }
	static void shutdown() { get_instance()._shutdown(); }
	static void load_asset(std::string filepath) { get_instance()._load_asset(filepath); }

	template<typename T>
	static std::shared_ptr<T> get_asset(std::string name) { return get_instance()._get_asset<T>(name); }

private:
	asset_manager() {}
	void _init();
	void _shutdown();
	void _load_asset(std::string filepath);

	template<typename T>
	std::shared_ptr<T> _get_asset(std::string name) const {
		return dynamic_pointer_cast<T>(_assets[name]);
	}

	std::unordered_map<std::string, std::shared_ptr<asset>> _assets;
};

