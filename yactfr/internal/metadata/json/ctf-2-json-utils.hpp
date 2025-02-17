/*
 * Copyright (C) 2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef _YACTFR_INTERNAL_METADATA_JSON_CTF_2_JSON_UTILS_HPP
#define _YACTFR_INTERNAL_METADATA_JSON_CTF_2_JSON_UTILS_HPP

#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>

#include <yactfr/metadata/item.hpp>

#include "json-val.hpp"
#include "../pseudo-types.hpp"

namespace yactfr {
namespace internal {

/*
 * Returns the UUID of the JSON object value `jsonObjVal`, or
 * `boost::none` if there's no such property.
 */
boost::optional<boost::uuids::uuid> uuidOfObj(const JsonObjVal& jsonObjVal);

/*
 * Returns the user attributes of the JSON object value `jsonObjVal`, or
 * an empty map item there's no such property.
 */
MapItem::UP userAttrsOfObj(const JsonObjVal& jsonObjVal);

/*
 * Returns the raw integer value from the JSON unsigned or signed
 * integer value `jsonIntVal`, casted as `ValT`.
 */
template <typename ValT>
ValT rawIntValFromJsonIntVal(const JsonVal& jsonIntVal) noexcept
{
    if (jsonIntVal.isUInt()) {
        return static_cast<ValT>(*jsonIntVal.asUInt());
    } else {
        return static_cast<ValT>(*jsonIntVal.asSInt());
    }
}

/*
 * Returns the optional raw string value from the property named
 * `propName` within `jsonObjVal`.
 */
static inline boost::optional<std::string> optStrOfObj(const JsonObjVal& jsonObjVal,
                                                       const std::string& propName)
{
    const auto jsonVal = jsonObjVal[propName];

    if (jsonVal) {
        return *jsonVal->asStr();
    }

    return boost::none;
}

} // namespace internal
} // namespace yactfr

#endif // _YACTFR_INTERNAL_METADATA_JSON_CTF_2_JSON_UTILS_HPP
