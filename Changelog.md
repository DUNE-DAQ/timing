# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).


## [Unreleased]
### Changed

### Added
- Support for the new fine-grained fake trigger generator


## [4.a.4] 2018-05-23
### Added
- Board/carrier/design type detection via `io.config` registers.
- `SFPExpander` class to simplify the `pc059` setup.

## [4.a.3] 2018-04-17
### Added
- Support for pc509

### Changed
- Fixed trigger rate reported by `faketrig-conf`

## [4.a.2] 2018-03-22
### Changed
- Renamed `triggen` and `trigclear` `mst` commands as `faketrig-conf` and `faketrig-clear` respectively.

### Added
- `click_didyoumean` among python package requirements.

## [4.a.1] 2018-03-21
### Changed
- Extended the `mst` `triggen` parameters with channel id to control up to 4 trigger generators in parallel. Each generator issues a dedicated `FakeTrigger` command (`FakeTrigger0`-`FakeTrigger3`).
- Added `readROBWarningOverflow` and `readROBError` `PartitionNode` methods.


## [4.a.0] 2018-03-15
### Changed
- Renamed `master` and `endpoint` to `mst` and `ept` respectively.
- Endpoint `ept` take a list or range of endpoint ids as first argument.

### Added
- This changelog.
- `master` partitition subcommand `part`.
- `trig` command to enable partition triggers.
- Added `PartitionNode::enableTriggers` method.
- Master firmware version check.
