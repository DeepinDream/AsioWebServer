#pragma once

namespace AsioWeb {

enum class DataProcState {
    DATA_BEGIN,
    DATA_CONTINUE,
    DATA_END,
    DATA_ALL_END,
    DATA_CLOSE,
    DATA_ERR
};

class ChunkedData {
  public:
    static constexpr const unsigned int k_MaxLength = 3 * 1024 * 1024;
    ChunkedData()
        : enabled_(false)
        , seek_pos_(0)
        , length_(0)
        , state_(DataProcState::DATA_BEGIN)
    {
    }

    void setEnabled(const bool enabled)
    {
        enabled_ = enabled;
    }
    bool getEnabled()
    {
        return enabled_;
    }

    void setSeekPos(const unsigned int pos)
    {
        seek_pos_ = pos;
    }
    unsigned int getSeekPos()
    {
        return seek_pos_;
    }

    void setLength(const unsigned int length)
    {
        length_ = length;
    }
    unsigned int getLength()
    {
        return length_;
    }

    void setProcState(const DataProcState state)
    {
        state_ = state;
    }
    DataProcState getProcState()
    {
        return state_;
    }

  private:
    bool enabled_;
    unsigned int seek_pos_;
    unsigned int length_;
    DataProcState state_;
};
}  // namespace AsioWeb