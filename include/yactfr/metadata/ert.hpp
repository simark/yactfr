/*
 * Copyright (C) 2015-2018 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef _YACTFR_METADATA_ERT_HPP
#define _YACTFR_METADATA_ERT_HPP

#include <string>
#include <memory>
#include <utility>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

#include "aliases.hpp"
#include "item.hpp"

namespace yactfr {
namespace internal {

class TraceTypeImpl;

} // namespace internal

class DataStreamType;
class DataType;
class StructureType;
class DataLocation;

/*!
@brief
    Numeric log level.

@ingroup metadata
*/
using LogLevel = long long;

/*!
@brief
    Event record type.

@ingroup metadata

An event record type describes data stream event records.
*/
class EventRecordType final :
    boost::noncopyable
{
    friend class internal::TraceTypeImpl;

public:
    /// Unique pointer to constant event record type.
    using UP = std::unique_ptr<const EventRecordType>;

public:
    /*!
    @brief
        Builds an event record type.

    @param[in] id
        Event record type ID.
    @param[in] nameSpace
        Namespace.
    @param[in] name
        Name.
    @param[in] logLevel
        Log level.
    @param[in] emfUri
        EMF URI.
    @param[in] specificContextType
        Specific context type, or \c nullptr if none.
    @param[in] payloadType
        Payload type, or \c nullptr if none.
    @param[in] userAttributes
        @parblock
        User attributes.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        If set, no data type in \p specificContextType, recursively, has
        a role (UnsignedIntegerTypeRole or "metadata stream UUID"
        role).
    @pre
        If set, no data type in \p payloadType, recursively, has a role
        (UnsignedIntegerTypeRole or "metadata stream UUID" role).
    */
    explicit EventRecordType(TypeId id, boost::optional<std::string> nameSpace,
                             boost::optional<std::string> name, boost::optional<LogLevel> logLevel,
                             boost::optional<std::string> emfUri,
                             std::unique_ptr<const StructureType> specificContextType,
                             std::unique_ptr<const StructureType> payloadType,
                             MapItem::UP userAttributes = nullptr);

    /*!
    @brief
        Creates a constant event record type unique pointer, forwarding
        \p args to the constructor.

    @param[in] args
        Arguments to forward to the event record type constructor.

    @returns
        Created constant event record type unique pointer.

    @pre
        See the preconditions of the constructor.
    */
    template <typename... ArgTs>
    static UP create(ArgTs&&... args)
    {
        return std::make_unique<UP::element_type>(std::forward<ArgTs>(args)...);
    }

    /// Numeric ID, unique amongst the IDs of all the event record types
    /// which are part of the same \link DataStreamType data stream
    /// type\endlink.
    TypeId id() const noexcept
    {
        return _id;
    }

    /// Namespace.
    const boost::optional<std::string>& nameSpace() const noexcept
    {
        return _ns;
    }

    /// Name.
    const boost::optional<std::string>& name() const noexcept
    {
        return _name;
    }

    /// Log level.
    const boost::optional<LogLevel>& logLevel() const noexcept
    {
        return _logLevel;
    }

    /// EMF URI.
    const boost::optional<std::string>& emfUri() const noexcept
    {
        return _emfUri;
    }

    /// Type of the context structure following the common context
    /// structure of the event records described by this type, or
    /// \c nullptr if there's no specific context type.
    const StructureType *specificContextType() const noexcept
    {
        return _specCtxType.get();
    }

    /// Type of the payload structure following the specific context
    /// structure of the event records described by this type, or
    /// \c nullptr if there's no payload type.
    const StructureType *payloadType() const noexcept
    {
        return _payloadType.get();
    }

    /// Parent data stream type of this type, or \c nullptr if this type
    /// is not part of a trace type yet.
    const DataStreamType *dataStreamType() const noexcept
    {
        return _dst;
    }

    /*!
    @brief
        User attributes.

    If set, each key of the returned map item is a namespace.

    @note
        Even if the return value isn't \c nullptr, the returned map
        item may still be empty (which also means no user attributes).
    */
    const MapItem *userAttributes() const noexcept
    {
        return _userAttrs.get();
    }

private:
    void _setDst(const DataStreamType& dst) const noexcept;

private:
    const TypeId _id;
    const boost::optional<std::string> _ns;
    const boost::optional<std::string> _name;
    const boost::optional<LogLevel> _logLevel;
    const boost::optional<std::string> _emfUri;
    std::unique_ptr<const StructureType> _specCtxType;
    std::unique_ptr<const StructureType> _payloadType;
    const MapItem::UP _userAttrs;
    mutable const DataStreamType *_dst = nullptr;
};

} // namespace yactfr

#endif // _YACTFR_METADATA_ERT_HPP
