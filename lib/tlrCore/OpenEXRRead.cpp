// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021 Darby Johnston
// All rights reserved.

#include <tlrCore/OpenEXR.h>

#include <tlrCore/StringFormat.h>

#include <ImfRgbaFile.h>

namespace tlr
{
    namespace exr
    {
        namespace
        {
            avio::Info imfInfo(const Imf::RgbaInputFile& f)
            {
                avio::Info out;
                imaging::PixelType pixelType = imaging::getFloatType(4, 16);
                if (imaging::PixelType::None == pixelType)
                {
                    throw std::runtime_error(string::Format("{0}: File not supported").arg(f.fileName()));
                }
                const auto dw = f.dataWindow();
                const int width = dw.max.x - dw.min.x + 1;
                const int height = dw.max.y - dw.min.y + 1;
                imaging::Info imageInfo(width, height, pixelType);
                imageInfo.layout.mirror.y = true;
                out.video.push_back(imageInfo);
                readTags(f.header(), out.tags);
                return out;
            }
        }

        void Read::_init(
            const file::Path& path,
            const avio::Options& options,
            const std::shared_ptr<core::LogSystem>& logSystem)
        {
            ISequenceRead::_init(path, options, logSystem);
        }

        Read::Read()
        {}

        Read::~Read()
        {
            _finish();
        }

        std::shared_ptr<Read> Read::create(
            const file::Path& path,
            const avio::Options& options,
            const std::shared_ptr<core::LogSystem>& logSystem)
        {
            auto out = std::shared_ptr<Read>(new Read);
            out->_init(path, options, logSystem);
            return out;
        }

        avio::Info Read::_getInfo(const std::string& fileName)
        {
            avio::Info out;
            Imf::RgbaInputFile f(fileName.c_str());
            out = imfInfo(f);
            float speed = _defaultSpeed;
            const auto i = out.tags.find("Frame Per Second");
            if (i != out.tags.end())
            {
                speed = std::stof(i->second);
            }
            out.videoTimeRange = otime::TimeRange::range_from_start_end_time_inclusive(
                otime::RationalTime(_startFrame, speed),
                otime::RationalTime(_endFrame, speed));
            out.videoType = avio::VideoType::Sequence;
            return out;
        }

        avio::VideoFrame Read::_readVideoFrame(
            const std::string& fileName,
            const otime::RationalTime& time,
            const std::shared_ptr<imaging::Image>& image)
        {
            Imf::RgbaInputFile f(fileName.c_str());
            const auto info = imfInfo(f);

            avio::VideoFrame out;
            out.time = time;
            out.image = image && image->getInfo() == info.video[0] ? image : imaging::Image::create(info.video[0]);
            out.image->setTags(info.tags);

            const auto dw = f.dataWindow();
            const int width = dw.max.x - dw.min.x + 1;
            const int height = dw.max.y - dw.min.y + 1;
            f.setFrameBuffer(
                reinterpret_cast<Imf::Rgba*>(out.image->getData()) - dw.min.x - dw.min.y * width,
                1,
                width);
            f.readPixels(dw.min.y, dw.max.y);

            return out;
        }
    }
}
