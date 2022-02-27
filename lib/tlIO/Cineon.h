// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlIO/SequenceIO.h>

#include <tlCore/FileIO.h>

namespace tl
{
    namespace io
    {
        //! Cineon I/O.
        //!
        //! References:
        //! - Kodak, "4.5 DRAFT - Image File Format Proposal for Digital Pictures"
        namespace cineon
        {
            //! Cineon header magic numbers.
            const uint32_t magic[] =
            {
                0x802a5fd7,
                0xd75f2a80
            };

            //! Cineon image orientations.
            enum class Orient
            {
                LeftRightTopBottom,
                LeftRightBottomTop,
                RightLeftTopBottom,
                RightLeftBottomTop,
                TopBottomLeftRight,
                TopBottomRightLeft,
                BottomTopLeftRight,
                BottomTopRightLeft,

                Count,
                First = LeftRightTopBottom
            };
            TLRENDER_ENUM(Orient);
            TLRENDER_ENUM_SERIALIZE(Orient);

            //! Cineon file descriptors.
            enum class Descriptor
            {
                Luminance,
                RedFilmPrint,
                GreenFilmPrint,
                BlueFilmPrint,
                RedCCIRXA11,
                GreenCCIRXA11,
                BlueCCIRXA11,

                Count,
                First = Luminance
            };
            TLRENDER_ENUM(Descriptor);
            TLRENDER_ENUM_SERIALIZE(Descriptor);

            //! Cineon header.
            struct Header
            {
                Header();

                //! File header.
                struct File
                {
                    uint32_t magic;
                    uint32_t imageOffset;
                    uint32_t headerSize;
                    uint32_t industryHeaderSize;
                    uint32_t userHeaderSize;
                    uint32_t size;
                    char     version[8];
                    char     name[100];
                    char     time[24];
                    uint8_t  pad[36];
                };
                File file;

                //! Image header.
                struct Image
                {
                    uint8_t orient;
                    uint8_t channels;
                    uint8_t pad[2];

                    //! Channel header.
                    struct Channel
                    {
                        uint8_t  descriptor[2];
                        uint8_t  bitDepth;
                        uint8_t  pad;
                        uint32_t size[2];
                        float    lowData;
                        float    lowQuantity;
                        float    highData;
                        float    highQuantity;
                    };
                    Channel channel[8];

                    float    white[2];
                    float    red[2];
                    float    green[2];
                    float    blue[2];
                    char     label[200];
                    uint8_t  pad2[28];
                    uint8_t  interleave;
                    uint8_t  packing;
                    uint8_t  dataSign;
                    uint8_t  dataSense;
                    uint32_t linePadding;
                    uint32_t channelPadding;
                    uint8_t  pad3[20];
                };
                Image image;

                //! Source header.
                struct Source
                {
                    int32_t offset[2];
                    char    file[100];
                    char    time[24];
                    char    inputDevice[64];
                    char    inputModel[32];
                    char    inputSerial[32];
                    float   inputPitch[2];
                    float   gamma;
                    char    pad[40];
                };
                Source source;

                //! Film header.
                struct Film
                {
                    uint8_t  id;
                    uint8_t  type;
                    uint8_t  offset;
                    uint8_t  pad;
                    uint8_t  prefix;
                    uint32_t count;
                    char     format[32];
                    uint32_t frame;
                    float    frameRate;
                    char     frameId[32];
                    char     slate[200];
                    char     pad2[740];
                };
                Film film;
            };

            //! Check whether the Cineon header value is valid.
            bool isValid(const char*, size_t size);

            //! Convert a Cineon header string to std::string.
            std::string toString(const char*, size_t size);

            //! Convert a std::string to a Cineon header string.
            size_t fromString(
                const std::string& string,
                char* out,
                size_t             maxLen,
                bool               terminate);

            //! Read a header.
            Header read(const std::shared_ptr<core::file::FileIO>&, Info&);

            //! Write a header.
            void write(const std::shared_ptr<core::file::FileIO>&, const Info&);

            //! Finish writing the header.
            void finishWrite(const std::shared_ptr<core::file::FileIO>&);

            //! Cineon reader.
            class Read : public ISequenceRead
            {
            protected:
                void _init(
                    const core::file::Path&,
                    const Options&,
                    const std::weak_ptr<core::LogSystem>&);
                Read();

            public:
                ~Read() override;

                //! Create a new reader.
                static std::shared_ptr<Read> create(
                    const core::file::Path&,
                    const Options&,
                    const std::weak_ptr<core::LogSystem>&);

            protected:
                Info _getInfo(const std::string& fileName) override;
                VideoData _readVideo(
                    const std::string& fileName,
                    const otime::RationalTime&,
                    uint16_t layer) override;
            };

            //! Cineon writer.
            class Write : public ISequenceWrite
            {
            protected:
                void _init(
                    const core::file::Path&,
                    const Info&,
                    const Options&,
                    const std::weak_ptr<core::LogSystem>&);
                Write();

            public:
                ~Write() override;

                //! Create a new writer.
                static std::shared_ptr<Write> create(
                    const core::file::Path&,
                    const Info&,
                    const Options&,
                    const std::weak_ptr<core::LogSystem>&);

            protected:
                void _writeVideo(
                    const std::string& fileName,
                    const otime::RationalTime&,
                    const std::shared_ptr<core::imaging::Image>&) override;
            };

            //! Cineon plugin.
            class Plugin : public IPlugin
            {
            protected:
                void _init(const std::weak_ptr<core::LogSystem>&);
                Plugin();

            public:
                //! Create a new plugin.
                static std::shared_ptr<Plugin> create(const std::weak_ptr<core::LogSystem>&);

                std::shared_ptr<IRead> read(
                    const core::file::Path&,
                    const Options & = Options()) override;
                core::imaging::Info getWriteInfo(
                    const core::imaging::Info&,
                    const Options & = Options()) const override;
                std::shared_ptr<IWrite> write(
                    const core::file::Path&,
                    const Info&,
                    const Options & = Options()) override;
            };
        }
    }
}
