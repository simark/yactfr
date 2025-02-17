/*
 * Copyright (C) 2015-2018 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef _YACTFR_METADATA_TRACE_TYPE_HPP
#define _YACTFR_METADATA_TRACE_TYPE_HPP

#include <memory>
#include <utility>
#include <string>
#include <boost/optional.hpp>

#include "clk-type.hpp"
#include "dst.hpp"
#include "dt.hpp"
#include "struct-type.hpp"
#include "item.hpp"
#include "trace-env.hpp"

namespace yactfr {
namespace internal {

class TraceTypeImpl;

struct ClkTypeNameComp final
{
    bool operator()(const std::unique_ptr<const ClockType>& a,
                    const std::unique_ptr<const ClockType>& b) const
    {
        return a->name() < b->name();
    }
};

struct DstIdComp final
{
    bool operator()(const std::unique_ptr<const DataStreamType>& a,
                    const std::unique_ptr<const DataStreamType>& b) const
    {
        return a->id() < b->id();
    }
};

} // namespace internal

/*!
@brief
    Set of clock types with unique names.

@ingroup metadata
*/
using ClockTypeSet = std::set<std::unique_ptr<const ClockType>, internal::ClkTypeNameComp>;

/*!
@brief
    Set of data stream types with unique IDs.

@ingroup metadata
*/
using DataStreamTypeSet = std::set<std::unique_ptr<const DataStreamType>, internal::DstIdComp>;

/*!
@brief
    Trace type.

@ingroup metadata

A trace type describes traces.
*/
class TraceType final :
    boost::noncopyable
{
    friend class internal::TraceTypeImpl;
    friend class ElementSequenceIterator;

public:
    /// Unique pointer to constant trace type.
    using UP = std::unique_ptr<const TraceType>;

public:
    /*!
    @brief
        Builds a trace type.

    @param[in] majorVersion
        Major version.
    @param[in] minorVersion
        Minor version.
    @param[in] uid
        Unique ID of traces described by this trace type.
    @param[in] environment
        Environment of traces described by this trace type.
    @param[in] packetHeaderType
        Packet header type, or \c nullptr if none.
    @param[in] clockTypes
        Clock types of the trace type (moved).
    @param[in] dataStreamTypes
        Data stream types (moved).
    @param[in] userAttributes
        @parblock
        User attributes.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        @parblock
        \p majorVersion and \p minorVersion are resp. 1 and 8, or
        2 and 0.

        If set, any unsigned integer type within \p packetHeaderType,
        recursively, may only have the following roles:

        - UnsignedIntegerTypeRole::PACKET_MAGIC_NUMBER
        - UnsignedIntegerTypeRole::DATA_STREAM_TYPE_ID
        - UnsignedIntegerTypeRole::DATA_STREAM_ID

        For each \link DataStreamType data stream type\endlink \em DST
        of \p dataStreamTypes, if \em DST has a default clock type, it
        points to one of the clock types of \p clockTypes.

        The data types of \p packetHeaderType and of
        \p dataStreamTypes, recursively, have
        \link DataLocation data locations\endlink which satisfy the
        requirements of \yactfr_ctf2.

        <strong>If \p majorVersion is 1</strong>, then:

        - \p packetHeaderType and all the data types within
          \p dataStreamTypes don't contain the following data types:

          - VariableLengthUnsignedIntegerType
          - VariableLengthSignedIntegerType
          - VariableLengthUnsignedEnumerationType
          - VariableLengthSignedEnumerationType
          - StaticLengthBlobType
          - DynamicLengthBlobType
          - OptionalWithBooleanSelectorType
          - OptionalWithUnsignedIntegerSelectorType
          - OptionalWithSignedIntegerSelectorType

        - \p userAttributes is not set.

        - All the \link ClockType clock types\endlink
          within \p clockTypes have no user attributes
          (ClockType::userAttributes() returns \c nullptr).

        - All the \link DataStreamType data stream types\endlink
          within \p dataStreamTypes have no user attributes
          (DataStreamType::userAttributes() returns \c nullptr).

        - All the \link EventRecordType event record types\endlink
          within \p dataStreamTypes have no user attributes
          (EventRecordType::userAttributes() returns \c nullptr).

        - All the \link DataType data types\endlink
          within \p dataStreamTypes have no user attributes
          (DataType::userAttributes() returns \c nullptr).

        <strong>If \p majorVersion is 2</strong>, then:

        - All contained \link EventRecordType event record types\endlink
          (in \p dataStreamTypes) have \em no log level
          (EventRecordType::logLevel() returns
          <code>boost::none</code>).

        - All contained \link EventRecordType event record types\endlink
          (in \p dataStreamTypes) have \em no EMF URI
          (EventRecordType::emfUri() returns <code>boost::none</code>).

        - If set, any
          \link StaticLengthArrayType static-length array type\endlink
          within \p packetHeaderType, recursively, doesn't have a
          "metadata stream UUID" role
          (StaticLengthArrayType::hasMetadataStreamUuidRole() returns
          \c false).
        @endparblock
    */
    explicit TraceType(unsigned int majorVersion, unsigned int minorVersion,
                       boost::optional<std::string> uid, TraceEnvironment environment,
                       StructureType::UP packetHeaderType, ClockTypeSet&& clockTypes,
                       DataStreamTypeSet&& dataStreamTypes, MapItem::UP userAttributes = nullptr);

    /*!
    @brief
        Creates a constant trace type unique pointer, forwarding \p args
        to the constructor.

    @param[in] args
        Arguments to forward to the trace type constructor.

    @returns
        Created constant trace type unique pointer.

    @pre
        See the preconditions of the constructor.
    */
    template <typename... ArgTs>
    static UP create(ArgTs&&... args)
    {
        return std::make_unique<UP::element_type>(std::forward<ArgTs>(args)...);
    }

    /*
     * Required because `internal::TraceTypeImpl` has no known size at
     * this point.
     */
    ~TraceType();

    /// Major version.
    unsigned int majorVersion() const noexcept;

    /// Minor version.
    unsigned int minorVersion() const noexcept;

    /// Trace unique ID.
    const boost::optional<std::string>& uid() const noexcept;

    /// Trace environment.
    const TraceEnvironment& environment() const noexcept;

    /// Type of the header structure of all the packets of the trace
    /// described by this type, or \c nullptr if there's no packet
    /// header type.
    const StructureType *packetHeaderType() const noexcept;

    /// Contained clock types.
    const ClockTypeSet& clockTypes() const noexcept;

    /// Contained data stream types.
    const DataStreamTypeSet& dataStreamTypes() const noexcept;

    /*!
    @brief
        User attributes.

    If set, each key of the returned map item is a namespace.

    @note
        Even if the return value isn't \c nullptr, the returned map
        item may still be empty (which also means no user attributes).
    */
    const MapItem *userAttributes() const noexcept;

    /*!
    @brief
        Returns the data stream type having the ID \p id,
        or \c nullptr if not found.

    @param[in] id
        ID of the data stream type to find.

    @returns
        Data stream type with ID \p id, or \c nullptr if not found.
    */
    const DataStreamType *operator[](TypeId id) const;

    /// Data stream type constant iterator set at the first data stream
    /// type of this type.
    DataStreamTypeSet::const_iterator begin() const noexcept;

    /// Data stream type constant iterator set \em after the last data
    /// stream type of this type.
    DataStreamTypeSet::const_iterator end() const noexcept;

    /// Number of data stream types this type has.
    Size size() const noexcept;

    /// Whether or not this type is empty (has no data stream types).
    bool isEmpty() const noexcept;

private:
    const std::unique_ptr<internal::TraceTypeImpl> _pimpl;
};

} // namespace yactfr

#endif // _YACTFR_METADATA_TRACE_TYPE_HPP
