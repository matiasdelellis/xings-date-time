# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.3.0] - 2026-09-09

### Added

- Asynchronous D-Bus calls: the UI never blocks on the system bus or polkit
  authentication, with a loading indicator for the time zone list
- Visible error dialogs when setting automatic time, time or time zone fails
- Time zone search: spaces match underscores, scroll to top on filter,
  placeholder on empty results, scroll to the current zone on open,
  double-click or Enter applies
- Error info bar shown only for real errors
- Hardware clock mode (local time) with automatic time support detection
- Live refresh when time settings change externally
- Application keywords and software center metadata (screenshots, releases)

### Fixed

- Memory leaks of D-Bus proxies, dialog builders and date objects
- Minute/second spinners not updating the preview label
- Crashes on invalid dates and on closed dialogs
- `--verbose` command line option handling
- Panel left half-initialized when the system bus is unreachable
- Manpage date and metainfo validation issues

## [0.2.0] - 2024-08-13

### Added

- Initial Release

