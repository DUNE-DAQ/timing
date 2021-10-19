/**
 * @file BoreasDesign.hpp
 *
 * BoreasDesign is a base class providing an interface
 * to top level master with integrated HSI firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_BOREASDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_BOREASDESIGN_HPP_

// Timing Headers
#include "timing/HSINode.hpp"
#include "timing/MasterDesign.hpp"
#include "timing/PDIMasterNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing master with integrated HSI designs.
 */
template<class IO>
class BoreasDesign : public MasterDesign<IO, PDIMasterNode>
{

public:
  explicit BoreasDesign(const uhal::Node& node);
  virtual ~BoreasDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Prepare the timing master for data taking.
   *
   */
  void configure() const override;

  template<class T>
  void get_info(T& data) const;

  /**
   * @brief      Get the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const HSINode& get_hsi_node() const { return uhal::Node::getNode<HSINode>("endpoint0"); }

  /**
   * @brief      Configure the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual void configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                             uint32_t re_mask,  // NOLINT(build/unsigned)
                             uint32_t fe_mask,  // NOLINT(build/unsigned)
                             uint32_t inv_mask, // NOLINT(build/unsigned)
                             double rate,
                             bool dispatch = true) const;


  // In leiu of UHAL_DERIVEDNODE
protected:
  virtual uhal::Node* clone() const;
  //
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/BoreasDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_BOREASDESIGN_HPP_