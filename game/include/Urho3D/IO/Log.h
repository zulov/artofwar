//
// Copyright (c) 2008-2022 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Container/List.h"
#include "../Core/Mutex.h"
#include "../Core/Object.h"
#include "../Core/StringUtils.h"

namespace Urho3D
{

/// Fictional message level to indicate a stored raw message.
static const int LOG_RAW = -1;
/// Trace message level.
static const int LOG_TRACE = 0;
/// Debug message level. By default only shown in debug mode.
static const int LOG_DEBUG = 1;
/// Informative message level.
static const int LOG_INFO = 2;
/// Warning message level.
static const int LOG_WARNING = 3;
/// Error message level.
static const int LOG_ERROR = 4;
/// Disable all log messages.
static const int LOG_NONE = 5;

class File;

/// Stored log message from another thread.
struct StoredLogMessage
{
    /// Construct undefined.
    StoredLogMessage() = default;

    /// Construct with parameters.
    StoredLogMessage(const String& message, int level, bool error) :
        message_(message),
        level_(level),
        error_(error)
    {
    }

    /// Message text.
    String message_;
    /// Message level. -1 for raw messages.
    int level_{};
    /// Error flag for raw messages.
    bool error_{};
};

/// Logging subsystem.
class URHO3D_API Log : public Object
{
    URHO3D_OBJECT(Log, Object);

public:
    /// Construct.
    explicit Log(Context* context);
    /// Destruct. Close the log file if open.
    ~Log() override;

    /// Open the log file.
    void Open(const String& fileName);
    /// Close the log file.
    void Close();
    /// Set logging level.
    /// @property
    void SetLevel(int level);
    /// Set whether to timestamp log messages.
    /// @property
    void SetTimeStamp(bool enable);
    /// Set quiet mode ie. only print error entries to standard error stream (which is normally redirected to console also). Output to log file is not affected by this mode.
    /// @property
    void SetQuiet(bool quiet);

    /// Return logging level.
    /// @property
    int GetLevel() const { return level_; }

    /// Return whether log messages are timestamped.
    /// @property
    bool GetTimeStamp() const { return timeStamp_; }

    /// Return last log message.
    /// @property
    String GetLastMessage() const { return lastMessage_; }

    /// Return whether log is in quiet mode (only errors printed to standard error stream).
    /// @property
    bool IsQuiet() const { return quiet_; }

    /// Write to the log. If logging level is higher than the level of the message, the message is ignored.
    /// @nobind
    static void Write(int level, const String& message);
    /// Write formatted message to the log. If logging level is higher than the level of the message, the message is ignored.
    static void WriteFormat(int level, const char* format, ...);
    /// Write raw output to the log.
    static void WriteRaw(const String& message, bool error = false);
    /// Write to the log using the FormatString formatting syntax. Requires at least one argument so that curly braces do not need to be escaped for unformatted Log::Write.
    template<typename Arg0, typename... Args>
    static void Write(int level, const char* format, Arg0&& arg0, Args&&... args)
    {
        Write(level, FormatString(format, std::forward<Arg0>(arg0), std::forward<Args>(args)...));
    }

private:
    /// Handle end of frame. Process the threaded log messages.
    void HandleEndFrame(StringHash eventType, VariantMap& eventData);

    /// Mutex for threaded operation.
    Mutex logMutex_;
    /// Log messages from other threads.
    List<StoredLogMessage> threadMessages_;
    /// Log file.
    SharedPtr<File> logFile_;
    /// Last log message.
    String lastMessage_;
    /// Logging level.
    int level_;
    /// Timestamp log messages flag.
    bool timeStamp_;
    /// In write flag to prevent recursion.
    bool inWrite_;
    /// Quiet mode flag.
    bool quiet_;
};

#ifdef URHO3D_LOGGING
#define URHO3D_LOG(level, message, ...) Urho3D::Log::Write(level, message, ##__VA_ARGS__)
#define URHO3D_LOGTRACE(message, ...) Urho3D::Log::Write(Urho3D::LOG_TRACE, message, ##__VA_ARGS__)
#define URHO3D_LOGDEBUG(message, ...) Urho3D::Log::Write(Urho3D::LOG_DEBUG, message, ##__VA_ARGS__)
#define URHO3D_LOGINFO(message, ...) Urho3D::Log::Write(Urho3D::LOG_INFO, message, ##__VA_ARGS__)
#define URHO3D_LOGWARNING(message, ...) Urho3D::Log::Write(Urho3D::LOG_WARNING, message, ##__VA_ARGS__)
#define URHO3D_LOGERROR(message, ...) Urho3D::Log::Write(Urho3D::LOG_ERROR, message, ##__VA_ARGS__)
#define URHO3D_LOGRAW(message, ...) Urho3D::Log::Write(Urho3D::LOG_RAW, message, ##__VA_ARGS__)

#define URHO3D_LOGF(level, format, ...) Urho3D::Log::WriteFormat(level, format, ##__VA_ARGS__)
#define URHO3D_LOGTRACEF(format, ...) Urho3D::Log::WriteFormat(Urho3D::LOG_TRACE, format, ##__VA_ARGS__)
#define URHO3D_LOGDEBUGF(format, ...) Urho3D::Log::WriteFormat(Urho3D::LOG_DEBUG, format, ##__VA_ARGS__)
#define URHO3D_LOGINFOF(format, ...) Urho3D::Log::WriteFormat(Urho3D::LOG_INFO, format, ##__VA_ARGS__)
#define URHO3D_LOGWARNINGF(format, ...) Urho3D::Log::WriteFormat(Urho3D::LOG_WARNING, format, ##__VA_ARGS__)
#define URHO3D_LOGERRORF(format, ...) Urho3D::Log::WriteFormat(Urho3D::LOG_ERROR, format, ##__VA_ARGS__)
#define URHO3D_LOGRAWF(format, ...) Urho3D::Log::WriteFormat(Urho3D::LOG_RAW, format, ##__VA_ARGS__)

#else
#define URHO3D_LOG(...) ((void)0)
#define URHO3D_LOGTRACE(...) ((void)0)
#define URHO3D_LOGDEBUG(...) ((void)0)
#define URHO3D_LOGINFO(...) ((void)0)
#define URHO3D_LOGWARNING(...) ((void)0)
#define URHO3D_LOGERROR(...) ((void)0)
#define URHO3D_LOGRAW(...) ((void)0)

#define URHO3D_LOGF(...) ((void)0)
#define URHO3D_LOGTRACEF(...) ((void)0)
#define URHO3D_LOGDEBUGF(...) ((void)0)
#define URHO3D_LOGINFOF(...) ((void)0)
#define URHO3D_LOGWARNINGF(...) ((void)0)
#define URHO3D_LOGERRORF(...) ((void)0)
#define URHO3D_LOGRAWF(...) ((void)0)
#endif

}
