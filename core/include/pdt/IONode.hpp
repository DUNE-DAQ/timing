#ifndef __PDT_IONODE_HPP__
#define __PDT_IONODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/exception.hpp"
#include "pdt/I2CMasterNode.hpp"
#include "pdt/I2CSlave.hpp"
#include "pdt/SI534xNode.hpp"
#include "pdt/I2CExpanderNode.hpp"
#include "pdt/DACNode.hpp"
#include "pdt/FrequencyCounterNode.hpp"
#include "pdt/I2CSFPNode.hpp"

namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class IONode : public TimingNode {
protected:
    const std::string mUIDI2CBus;
    const std::string mUIDI2CDevice;
    const std::string mPLLI2CBus;
    const std::string mPLLI2CDevice;
    const std::vector<std::string> mClockNames;
    const std::vector<std::string> mSFPI2CBuses;


    /**
     * @brief      Reset timing node.
     */
    virtual void writeSoftResetRegister() const;
public:
    IONode(const uhal::Node& aNode, std::string aUIDI2CBus, std::string aUIDI2CDevice, std::string aPLLI2CBus, std::string aPLLI2CDevice, std::vector<std::string> aClockNames, std::vector<std::string> aSFPI2CBuses);
    virtual ~IONode();

    /**
     * @brief      Read the word identifying the timing board.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t readBoardType() const;

    /**
     * @brief      Read the word identifying the FPFA carrier board.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t readCarrierType() const;

    /**
     * @brief      Read the word identifying the firmware design in the FPGA.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t readDesignType() const;

    /**
     * @brief      Read the word containing the timing board UID.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t readBoardUID() const;

    /**
     * @brief      Read the word identifying the timing board.
     *
     * @return     { description_of_the_return_value }
     */
    virtual BoardRevision getBoardRevision() const;

    /**
     * @brief      Print hardware information
     */
    virtual std::string getHardwareInfo(bool aPrint=false) const;

    /**
     * @brief      Get the full config path.
     *
     * @return     { description_of_the_return_value }
     */
    virtual std::string getFullClockConfigFilePath(const std::string& aClockConfigFile, int32_t aMode=-1) const;

    /**
     * @brief      Get the an I2C chip.
     *
     * @return     { description_of_the_return_value }
     */
    template < class T> 
    std::unique_ptr<const T> getI2CDevice(const std::string& i2cBusName, const std::string& i2cDeviceName) const;

    /**
     * @brief      Get the PLL chip.
     *
     * @return     { description_of_the_return_value }
     */
    virtual std::unique_ptr<const SI534xSlave> getPLL() const;

    /**
     * @brief      Configure clock chip.
     */
    virtual void configurePLL(const std::string& aClockConfigFile="") const;

    /**
     * @brief      Read frequencies of on-board clocks.
     */
    virtual std::vector<double> readClockFrequencies() const;

    /**
     * @brief      Print frequencies of on-board clocks.
     */
    virtual std::string getClockFrequenciesTable(bool aPrint=false) const;

    /**
     * @brief      Print status of on-board PLL.
     */
    virtual std::string getPLLStatus(bool aPrint=false) const;

    /**
     * @brief      Print status of on-board SFP.
     */
    virtual std::string getSFPStatus(uint32_t aSFPId, bool aPrint=false) const;

    /**
     * @brief      control tx laser of on-board SFP softly (I2C command)
     */
    virtual void switchSFPSoftTxControl(uint32_t aSFPId, bool aOn) const;

    /**
     * @brief      Reset timing node.
     */
    virtual void softReset() const;

    /**
     * @brief      Reset timing node.
     */
    virtual void reset(const std::string& aClockConfigFile="") const = 0;
};

} // namespace pdt

#include "pdt/IONode.hxx"

#endif // __PDT_IONODE_HPP__