/**
 * craft_extract - Copyright (c) 2022 atom0s [atom0s@live.com]
 *
 * This file is part of craft_extract.
 *
 * craft_extract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * craft_extract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with craft_extract.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CRAFT_EXTRACT_V67_HPP
#define CRAFT_EXTRACT_V67_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "defines.hpp"
#include "json.hpp"

#include "sqlite3.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/Backup.h"

namespace craft_extract::parser::v67
{
    /**
     * List of base material names.
     */
    const std::vector<std::string> base_materials{
        /**/ "", "cloth", "leather", "wood", "metal", "organic", "paper", "bronze", "iron", "mithril",
        /**/ "asterite", "glass", "stone", "laen", "alloy", "steel", "soft", "rawhide", "tanned", "cured",
        /**/ "hard", "rigid", "embossed", "imbued", "runed", "eldritch", "fine alloy", "", "", "adamantium",
        /**/ "birch", "rowan", "elm", "oaken", "ironwood", "heartwood", "runewood", "stonewood", "ebonwood", "dyrwood",
        /**/ "homespun", "woolen", "linen", "brocade", "silk", "gossamer", "sylvan", "seamist", "nightshade", "wyvernskin",
        /**/ "leaf", "bone", "vine", "shell", "fossil", "amber", "coral", "chitin", "copper", "ferrite",
        /**/ "quartz", "dolomite", "cobalt", "carbide", "sapphire", "diamond", "netherite", "arcanite", "netherium", "arcanium",
        /**/ "tempered", "duskwood", "silksteel", "petrified", "crystalized", "emerald", "sapphire", "ruby", "diamond", "aurulite",
        /**/ "raw", "uncut", "rough", "flawed", "imperfect", "polished", "faceted", "precious", "flawless", "perfect",
        /**/ "", "", "", "", "", "", "", "", "", "",
        /**/ "", "rough clout", "rough", "clout", "rough flight", "standard", "footed clout", "flight", "footed", "footed flight",
        /**/ "keen footed flight", "blunt footed flight", "barbed footed flight", "", "", "", "", "", "", ""};

    /**
     * List of realm names.
     */
    const std::vector<std::string> realm_names{"Albion", "Midgard", "Hibernia"};

    /**
     * Craft File Structure Definitions
     */

    struct material_t
    {
        uint32_t name_index;
        uint16_t count;
        uint16_t base_material;
    };

    struct recipe_t
    {
        uint32_t id;
        uint32_t name_index;
        uint32_t unknown00;
        uint16_t icon;
        uint16_t skill;
        uint16_t material_level;
        uint16_t level;
        uint16_t unknown01;
        uint16_t base_material;
        v67::material_t materials[8];
    };

    struct category_t
    {
        uint32_t name_index;
        uint16_t recipe_ids[50];
    };

    struct profession_t
    {
        uint16_t name_index;
        uint16_t unknown;
        uint16_t index;
        uint16_t index_list[201];
    };

    struct professions_t
    {
        v67::profession_t professions[20];
    };

    struct realminfo_t
    {
        uint32_t recipe_count;
        uint32_t category_count;
        uint32_t recipe_list_offset;
        uint32_t category_list_offset;
        uint32_t profession_list_offset;
    };

    struct header_t
    {
        uint32_t version;
        uint32_t strings_block_size;
        uint32_t strings_count;
        uint32_t strings_offset;
        v67::realminfo_t realms[3];
    };

    /**
     * Parsed Crafting Recipe Structure Definitions
     */

    struct craftmaterial_t
    {
        uint16_t base_material;
        uint16_t count;
        uint32_t name_index;
    };

    struct craft_t
    {
        uint32_t name_index_realm;
        uint32_t name_index_profession;
        uint32_t name_index_category;
        uint32_t name_index_recipe;

        uint32_t base_material;
        uint16_t icon;
        uint32_t id;
        uint16_t level;
        uint16_t material_level;
        uint16_t skill;

        std::vector<v67::craftmaterial_t> materials;
    };

    /**
     * Craft information containers.
     */
    std::vector<std::string> strings;
    std::map<uint32_t, std::vector<v67::craft_t>> crafts;

    /**
     * Parses the current file for craft information.
     *
     * @param {FILE*} f - File pointer of the current opened input file.
     * @param {std::size_t} size - The size of the input file.
     * @return {bool} True on success, false otherwise.
     */
    bool parse(FILE* f, const std::size_t size)
    {
        crafts.clear();
        strings.clear();

        // Validate the file size..
        if (size < sizeof(v67::header_t))
        {
            std::cout << "[!] Error: Input file too small; cannot fully parse." << std::endl;
            return false;
        }

        // Read and validate the file header..
        v67::header_t header{};
        ::fread(&header, sizeof(v67::header_t), 1, f);

        if (header.version != 0x67)
        {
            std::cout << std::format("[!] Error: Invalid file header version; expected 0x67, got: {}", header.version) << std::endl;
            return false;
        }

        // Prepare and read the string information..
        std::vector<char> strings_data(header.strings_block_size, '\0');
        std::vector<uint32_t> strings_index_table(header.strings_count, 0);

        ::fseek(f, sizeof(v67::header_t) + header.strings_offset, SEEK_SET);
        ::fread(strings_data.data(), 1, header.strings_block_size, f);
        ::fread(strings_index_table.data(), 4, header.strings_count, f);

        // Parse the strings table strings..
        for (auto x = 1; x < header.strings_count; x++)
            strings.push_back(std::string(strings_data.data() + strings_index_table[x - 1], strings_index_table[x] - strings_index_table[x - 1] - 1));
        strings.push_back(std::string(strings_data.data() + strings_index_table.back(), (header.strings_block_size - 3) - strings_index_table[strings_index_table.size() - 1]));

        if (strings.size() != header.strings_count)
        {
            std::cout << "[!] Error: Failed to parse string table information." << std::endl;
            return false;
        }

        std::map<uint32_t, v67::professions_t> professions;
        std::map<uint32_t, std::vector<v67::recipe_t>> recipes;
        std::map<uint32_t, std::vector<v67::category_t>> categories;

        // Parse the crafting information for each realm..
        for (auto realm = 0; realm < 3; realm++)
        {
            const auto& rdata = header.realms[realm];

            // Read the professions table..
            ::fseek(f, rdata.profession_list_offset, SEEK_SET);
            ::fread(&professions[realm], sizeof(v67::professions_t), 1, f);

            // Read the recipes table..
            recipes[realm].resize(rdata.recipe_count);
            ::fseek(f, rdata.recipe_list_offset, SEEK_SET);
            ::fread(recipes[realm].data(), sizeof(v67::recipe_t) * rdata.recipe_count, 1, f);

            // Read the categories table..
            categories[realm].resize(rdata.category_count);
            ::fseek(f, rdata.category_list_offset, SEEK_SET);
            ::fread(categories[realm].data(), sizeof(v67::category_t) * rdata.category_count, 1, f);
        }

        // Process recipes for each realm..
        for (auto r = 0; r < realm_names.size(); r++)
        {
            const auto realm_name = realm_names[r];

            // Process each profession..
            for (auto p = 0; p < _countof(v67::professions_t::professions); p++)
            {
                if (p != 0 && professions[r].professions[p].index == 0)
                    continue;

                const auto p_nindex = professions[r].professions[p].name_index;
                if (p_nindex == 0)
                    continue;

                // Process each professions list of recipes..
                for (auto i = 1; i < _countof(v67::profession_t::index_list); i++)
                {
                    const auto pidx = professions[r].professions[p].index_list[i];
                    if (pidx == 0)
                        continue;

                    const auto cidx = categories[r][pidx].name_index;
                    if (cidx == 0)
                        continue;

                    for (auto c = 0; c < _countof(v67::category_t::recipe_ids); c++)
                    {
                        const auto rid = categories[r][pidx].recipe_ids[c];
                        if (rid == 0)
                            continue;

                        const auto ridx = recipes[r][rid].name_index;
                        if (ridx == 0)
                            continue;

                        const auto recipe = recipes[r][rid];

                        // Skip recipes that have no materials..
                        if (std::ranges::all_of(recipe.materials, [](auto m) -> bool { return m.count == 0; }))
                            continue;

                        // Prepare the craft recipe entry..
                        v67::craft_t craft{};
                        craft.name_index_realm      = r;
                        craft.name_index_profession = p_nindex;
                        craft.name_index_category   = cidx;
                        craft.name_index_recipe     = ridx;
                        craft.base_material         = recipe.base_material;
                        craft.icon                  = recipe.icon;
                        craft.id                    = recipe.id;
                        craft.level                 = recipe.level;
                        craft.material_level        = recipe.material_level;
                        craft.skill                 = recipe.skill;

                        // Add the craft recipe materials..
                        for (auto m = 0; m < _countof(v67::recipe_t::materials); m++)
                        {
                            const auto mat = recipe.materials[m];
                            if (mat.count == 0)
                                continue;

                            v67::craftmaterial_t material{};
                            material.base_material = mat.base_material;
                            material.count         = mat.count;
                            material.name_index    = mat.name_index;

                            craft.materials.push_back(material);
                        }

                        // Store the craft recipe entry..
                        crafts[r].push_back(craft);
                    }
                }
            }
        }

        return true;
    }

    /**
     * Saves the current parsed craft recipes to a csv file.
     *
     * @param {std::string} path - The output file to store the parsed information.
     * @return {bool} True on success, false otherwise.
     */
    bool save_csv(const std::string& path)
    {
        // Open the output file for writing..
        std::ofstream ofs(path);
        if (!ofs.is_open())
        {
            std::cout << "[!] Failed to open output file for writing!" << std::endl;
            return false;
        }

        // Write the main csv header row..
        ofs << "id, realm, realm_name, profession, category, name, base_material, base_material_name, icon, level, material_level, skill, mat1_base_material, mat1_base_material_name, mat1_count, mat1_name, mat2_base_material, mat2_base_material_name, mat2_count, mat2_name, mat3_base_material, mat3_base_material_name, mat3_count, mat3_name, mat4_base_material, mat4_base_material_name, mat4_count, mat4_name, mat5_base_material, mat5_base_material_name, mat5_count, mat5_name, mat6_base_material, mat6_base_material_name, mat6_count, mat6_name, mat7_base_material, mat7_base_material_name, mat7_count, mat7_name, mat8_base_material, mat8_base_material_name, mat8_count, mat8_name" << std::endl;

        // Write the recipes..
        for (auto iter = crafts.begin(), iterend = crafts.end(); iter != iterend; ++iter)
        {
            for (auto riter = iter->second.begin(), riterend = iter->second.end(); riter != riterend; ++riter)
            {
                ofs << std::format(
                    "{},{},{},{},{},{},{},{},{},{},{},{}",
                    riter->id,
                    iter->first,
                    realm_names[iter->first],
                    strings[riter->name_index_profession],
                    strings[riter->name_index_category],
                    strings[riter->name_index_recipe],
                    riter->base_material,
                    riter->base_material > 0 ? base_materials[riter->base_material] : "",
                    riter->icon,
                    riter->level,
                    riter->material_level,
                    riter->skill);

                for (const auto& m : riter->materials)
                {
                    ofs << std::format(",{},{},{},{}",
                        m.base_material,
                        m.base_material > 0 ? base_materials[m.base_material] : "",
                        m.count,
                        strings[m.name_index]);
                }

                ofs << std::endl;
            }
        }

        ofs.close();

        return true;
    }

    /**
     * Saves the current parsed craft recipes to a JSON file.
     *
     * @param {std::string} path - The output file to store the parsed information.
     * @return {bool} True on success, false otherwise.
     */
    bool save_json(const std::string& path)
    {
        try
        {
            nlohmann::json j;

            // Build the json file of recipes..
            for (auto iter = crafts.begin(), iterend = crafts.end(); iter != iterend; ++iter)
            {
                j[realm_names[iter->first]] = {};

                for (auto riter = iter->second.begin(), riterend = iter->second.end(); riter != riterend; ++riter)
                {
                    nlohmann::json r;
                    r["profession"] = strings[riter->name_index_profession];
                    r["category"]   = strings[riter->name_index_category];
                    r["name"]       = strings[riter->name_index_recipe];

                    if (riter->base_material == 0)
                        r["base_material_name"] = "";
                    else
                        r["base_material_name"] = base_materials[riter->base_material];

                    r["base_material"]  = riter->base_material;
                    r["icon"]           = riter->icon;
                    r["id"]             = riter->id;
                    r["level"]          = riter->level;
                    r["material_level"] = riter->material_level;
                    r["skill"]          = riter->skill;
                    r["materials"]      = {};

                    for (const auto& m : riter->materials)
                    {
                        nlohmann::json mat;

                        if (m.base_material == 0)
                            mat["base_material_name"] = "";
                        else
                            mat["base_material_name"] = base_materials[m.base_material];

                        mat["base_material"] = m.base_material;
                        mat["count"]         = m.count;
                        mat["name"]          = strings[m.name_index];

                        r["materials"] += mat;
                    }

                    j[realm_names[iter->first]] += r;
                }
            }

            // Open the output file for writing..
            std::ofstream ofs(path);
            if (!ofs.is_open())
            {
                std::cout << "[!] Failed to open output file for writing!" << std::endl;
                return false;
            }

            // Write the json data..
            ofs << j.dump(2);

            ofs.close();

            return true;
        }
        catch (const std::exception& e)
        {
            std::cout << "[!] Failed to save json file; caught exception:"
                      << std::endl
                      << std::endl
                      << e.what()
                      << std::endl;

            return false;
        }
    }

    /**
     * Saves the current parsed craft recipes to an SQLite database file.
     *
     * @param {std::string} path - The output file to store the parsed information.
     * @return {bool} True on success, false otherwise.
     */
    bool save_sqlite(const std::string& path)
    {
        SQLite::Database db(":memory:", SQLite::OPEN_READWRITE);

        // Prepare the various database tables..
        db.exec("CREATE TABLE about_craft_extract (created_by TEXT, paypal TEXT, github TEXT, patreon TEXT, repo TEXT);");
        db.exec("CREATE TABLE base_materials (id INT, name TEXT);");
        db.exec("CREATE TABLE realms (id INT, name TEXT);");
        db.exec("CREATE TABLE recipes (id INT, realm_id INT, profession TEXT, category TEXT, name TEXT, base_material INT, icon INT, level INT, material_level INT, skill INT);");
        db.exec("CREATE TABLE recipes_materials (recipe_id INT, base_material INT, count INT, name TEXT);");

        // Write the credits information..
        db.exec("INSERT INTO about_craft_extract VALUES('atom0s', 'https://paypal.me/atom0s', 'https://github.com/sponsors/atom0s', 'https://patreon.com/atom0s', 'https://github.com/atom0s/craft_extract');");

        // Write the base materials information..
        for (auto x = 0; x < base_materials.size(); x++)
            db.exec(std::format("INSERT INTO base_materials VALUES({}, \"{}\");", x, base_materials[x]));

        // Write the realms information..
        for (auto x = 0; x < realm_names.size(); x++)
            db.exec(std::format("INSERT INTO realms VALUES({}, \"{}\");", x, realm_names[x]));

        // Write the recipes information..
        for (auto iter = crafts.begin(), iterend = crafts.end(); iter != iterend; ++iter)
        {
            for (auto riter = iter->second.begin(), riterend = iter->second.end(); riter != riterend; ++riter)
            {
                db.exec(std::format("INSERT INTO recipes VALUES({}, {}, \"{}\", \"{}\", \"{}\", {}, {}, {}, {}, {});",
                    riter->id,
                    iter->first,
                    strings[riter->name_index_profession],
                    strings[riter->name_index_category],
                    strings[riter->name_index_recipe],
                    riter->base_material,
                    riter->icon,
                    riter->level,
                    riter->material_level,
                    riter->skill));

                for (const auto& m : riter->materials)
                {
                    db.exec(std::format("INSERT INTO recipes_materials VALUES({}, {}, {}, \"{}\");",
                        riter->id,
                        m.base_material,
                        m.count,
                        strings[m.name_index]));
                }
            }
        }

        // Backup the database to the output file..
        SQLite::Database bdb(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        SQLite::Backup backup(bdb, db);

        std::cout << "[!] Saving database, please wait..." << std::endl;

        const auto calcp = [](float min, float max) { return 100 - (min * 100 / max); };

        auto status = backup.executeStep(10);
        while (1)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ns);

            const auto r = backup.getRemainingPageCount();
            const auto t = backup.getTotalPageCount();

            std::cout << std::format("[!] Writing database to disk; pages remaining: {} / {} ({:.2f}%%)", r, t, calcp(r, t)) << std::endl;

            if (status == SQLITE_DONE)
                break;

            if (status != SQLITE_OK)
            {
                std::cout << std::format("[!] Error occurred while saving database. Status code: {}", status) << std::endl;
                break;
            }

            status = backup.executeStep(10);
        }

        return true;
    }

    /**
     * Saves the current parsed craft recipes to a plain-text file.
     *
     * @param {std::string} path - The output file to store the parsed information.
     * @return {bool} True on success, false otherwise.
     */
    bool save_text(const std::string& path)
    {
        // Open the output file for writing..
        std::ofstream ofs(path);
        if (!ofs.is_open())
        {
            std::cout << "[!] Failed to open output file for writing!" << std::endl;
            return false;
        }

        // Write the credits information..
        ofs << "//" << std::endl
            << "// File generated using craft_exporter by atom0s." << std::endl
            << "//" << std::endl
            << "// Contact  : https://atom0s.com/" << std::endl
            << "// Contact  : https://twitter.com/atom0s" << std::endl
            << "// Contact  : https://discord.gg/UmXNvjq - atom0s#0001" << std::endl
            << "// Donations: https://www.paypal.me/atom0s" << std::endl
            << "// Donations: https://github.com/sponsors/atom0s" << std::endl
            << "// Donations: https://patreon.com/atom0s" << std::endl
            << "//" << std::endl
            << std::endl;

        auto total_recipes = 0;
        for (const auto& r : crafts)
            total_recipes += r.second.size();

        ofs << "//" << std::endl
            << std::format("// Total Recipes: {}", total_recipes) << std::endl;

        for (const auto& r : crafts)
            ofs << std::format("//   - {}: {}", realm_names[r.first], r.second.size()) << std::endl;

        ofs << "//" << std::endl
            << std::endl;

        // Write the recipe information..
        for (auto iter = crafts.begin(), iterend = crafts.end(); iter != iterend; ++iter)
        {
            ofs << std::format("REALM: {}", realm_names[iter->first])
                << std::endl
                << std::endl;

            for (auto riter = iter->second.begin(), riterend = iter->second.end(); riter != riterend; ++riter)
            {
                std::stringstream ss;

                ss << std::format("    {} - {} - {} - ",
                    realm_names[riter->name_index_realm],
                    strings[riter->name_index_profession],
                    strings[riter->name_index_category]);

                if (riter->base_material > 0)
                {
                    const auto bmaterial = base_materials[riter->base_material];
                    if (bmaterial.size() > 0)
                        ss << bmaterial + " ";
                }

                ss << std::format("{} (MLv. {}) [Id: {}][Level: {}][Icon: {}][Skill: {}]",
                          strings[riter->name_index_recipe],
                          riter->material_level,
                          riter->id,
                          riter->level,
                          riter->icon,
                          riter->skill)
                   << std::endl;

                for (const auto& m : riter->materials)
                {
                    ss << std::format("      - {}x ", m.count);

                    if (m.base_material > 0)
                    {
                        const auto bmaterial = base_materials[m.base_material];
                        if (bmaterial.size() > 0)
                            ss << bmaterial + " ";
                    }

                    ss << std::format("{}", strings[m.name_index])
                       << std::endl;
                }

                ofs << ss.str() << std::endl;
            }

            ofs << std::endl;
        }

        ofs.close();

        return true;
    }

    /**
     * Saves the parsed craft recipe information to the desired output file.
     *
     * @param {std::string} path - The output file to store the parsed information.
     * @param {output_mode} mode - The output file format to use when saving.
     * @return {bool} True on success, false otherwise.
     */
    bool save(const std::string& path, const craft_extract::output_mode mode)
    {
        switch (mode)
        {
            case craft_extract::output_mode::csv:
                return save_csv(path);
            case craft_extract::output_mode::json:
                return save_json(path);
            case craft_extract::output_mode::sqlite:
                return save_sqlite(path);
            case craft_extract::output_mode::text:
                return save_text(path);
        }

        return false;
    }

} // namespace craft_extract::parser::v67

#endif // CRAFT_EXTRACT_V67_HPP
