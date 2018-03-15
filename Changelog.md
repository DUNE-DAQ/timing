# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [4.a.0]
### Changed
- Renamed `master` and `endpoint` to `mst` and `ept` respectively.
- Endpoint `ept` take a list or range of endpoint ids as first argument.

### Added
- This changelog.
- `master` partitition subcommand `part`.
- `trig` command to enable partition triggers.
- Added `PartitionNode::enableTriggers` method.
- Master firmware version check.
