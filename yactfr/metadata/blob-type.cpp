/*
 * Copyright (C) 2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <cassert>
#include <utility>

#include <yactfr/metadata/blob-type.hpp>

namespace yactfr {

BlobType::BlobType(const _Kind kind, const unsigned int align, MapItem::UP userAttrs,
                   std::string&& mediaType) :
    ScalarDataType {kind, align, std::move(userAttrs)},
    _mediaType {std::move(mediaType)}
{
    assert(align >= 8);
}

bool BlobType::_isEqual(const DataType& other) const noexcept
{
    const auto& otherBlobType = static_cast<const BlobType&>(other);

    return _mediaType == otherBlobType._mediaType;
}

} // namespace yactfr
