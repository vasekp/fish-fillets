-- -----------------------------------------------------------------
-- These function are called when save or load is made.
-- script_save() ... calls level_save(serialized_level)
-- script_load() ... calls level_load(saved_moves)
--                   with global variable saved_moves
-- script_loadState() ... uses global variable saved_models
--                        to restore model states
-- -----------------------------------------------------------------

file_include("script/share/Pickle.lua")

function script_save()
    local serialized = pickle(getModelsTable())
    level_save(serialized)
end

function script_load()
    if not saved_moves then
        error("global variable 'saved_moves' is not set")
    end
    level_load(saved_moves)
end

local function assignModelAttributes(saved_table)
    local models = getModelsTable()
    --NOTE: don't save objects with cross references
    --NOTE: objects addresses will be different after load
    for model_key, model in pairs(saved_table) do
        for param_key, param in pairs(model) do
            models[model_key][param_key] = param
        end
    end
end

function script_loadState()
    if not saved_models then
        error("global variable 'saved_models' is not set")
    end
    local saved_table = unpickle_table(saved_models)
    assignModelAttributes(saved_table)
end
