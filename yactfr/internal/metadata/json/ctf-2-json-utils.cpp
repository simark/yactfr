/*
 * Copyright (C) 2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <cassert>

#include "ctf-2-json-utils.hpp"
#include "ctf-2-json-strs.hpp"
#include "item-from-json-val.hpp"

namespace yactfr {
namespace internal {

namespace buuids = boost::uuids;

boost::optional<buuids::uuid> uuidOfObj(const JsonObjVal& jsonObjVal)
{
    const auto jsonUuidVal = jsonObjVal[strs::UUID];

    if (!jsonUuidVal) {
        return boost::none;
    }

    buuids::uuid uuid;
    auto it = uuid.begin();

    for (auto i = 0U; i < uuid.static_size(); ++i, ++it) {
        *it = *jsonUuidVal->asArray()[i].asUInt();
    }

    return uuid;
}

MapItem::UP userAttrsOfObj(const JsonObjVal& jsonObjVal)
{
    const auto jsonUserAttrsVal = jsonObjVal[strs::USER_ATTRS];

    if (!jsonUserAttrsVal) {
        return createItem(MapItem::Container {});
    }

    return MapItem::UP {static_cast<const MapItem *>(itemFromJsonVal(*jsonUserAttrsVal).release())};
}

} // namespace internal
} // namespace yactfr
