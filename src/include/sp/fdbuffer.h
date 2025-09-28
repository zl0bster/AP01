#pragma once
//-------------------------------------
/* Class FrequencyDomainBuffer has a ring structure
 It may be used for
*  - 1 accept current fft frame and distribute it o workers
*  - 2 provide access to specified frames by ID
*  - 3 contains frame ID unordered map with frame links in buffer
*/
//--------------------------------------


#include <string>
#ifndef FDBUFFER_H
#define FDBUFFER_H

#include <array>
#include <cassert>
#include <span>
#include <unordered_map>
#include <fstream>

namespace fdb {
struct FD_FileHeader{
    // const char dataType[] ("FDB1") ;
    size_t frameSize;
    size_t framesQty;
    size_t complexCellSize;
    long startFrameID;
};

const int WrongPosCode = -1;
template <typename T = float, size_t FrameSize = 1024, size_t BufferSize = 80>
class FDBuffer {
  using FRAME_T = std::array<T, FrameSize>;
  std::array<FRAME_T, BufferSize> m_buf;
  // std::array< T, FrameSize*BufferSize> m_buf;
  std::unordered_map<long, int> m_register; // <ID , pos in buffer>
  long m_idCtr{0};
  int m_currentCell{WrongPosCode};
  int m_actualStartPos{WrongPosCode};
  bool m_empty{true};

public:
  FDBuffer(long id = 0) : m_idCtr{id} {}; // initial frame ID may be defined
  long currentFrameID() const {
    assert(!m_empty);
    return m_idCtr;
  }
  long actualStartID() const {
    assert(!m_empty);
    return getIDbyPos(m_actualStartPos);
  };
  std::span<T> currentFrame(); // provides rw access to current frame
  std::span<T> frame(long id); // provides rw access to frame by ID
  bool contains(long id) const {return m_register.contains(id);} // checks the ID is present
  void switchNextFrame(); // switches to the next frame
  bool saveBuffer(std::string fileName);
private:
  long getIDbyPos(int p) const; // returns "WrongPos" if not found
};

        //-----------------------------------------------------------------

        template <typename T, size_t FrameSize, size_t BufferSize>
        inline std::span<T> FDBuffer<T, FrameSize, BufferSize>::currentFrame() {
          assert(!m_empty);
          return std::span<T>(m_buf[m_currentCell]);
        }

        template <typename T, size_t FrameSize, size_t BufferSize>
        inline std::span<T> FDBuffer<T, FrameSize, BufferSize>::frame(long id) {
          assert(!m_empty);
          return std::span<T>(m_buf[m_register[id]]);
        }

        template <typename T, size_t FrameSize, size_t BufferSize>
        inline void FDBuffer<T, FrameSize, BufferSize>::switchNextFrame() {
            if (m_currentCell == WrongPosCode) {
            m_currentCell = 0;
            m_actualStartPos = 0;
            m_empty = false;
            m_register.emplace(m_idCtr, m_currentCell);
            return;
          }
          ++m_idCtr;
          ++m_currentCell;
          m_currentCell %= BufferSize;
          auto cellToErase = getIDbyPos(m_currentCell);
          if (cellToErase != WrongPosCode) {
            m_register.erase(cellToErase);
            ++m_actualStartPos;
            m_actualStartPos %= BufferSize;
          }
          m_register.emplace(m_idCtr, m_currentCell);
        }

        template <typename T, size_t FrameSize, size_t BufferSize>
        bool FDBuffer<T, FrameSize, BufferSize>::saveBuffer(std::string fileName)
        {
// #TODO write save file procedure - header and data
        }

        template <typename T, size_t FrameSize, size_t BufferSize>
        inline long
        FDBuffer<T, FrameSize, BufferSize>::getIDbyPos(int p) const {
          for (auto &cell : m_register) {
            if (cell.second == p) {
              return cell.first;
            }
          }
          return WrongPosCode;
        }
        } // namespace fdb

#endif // TDBUFFER_H
