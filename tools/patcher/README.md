# NESClone patch manager

The patch manager will handle application packages and metadata, not patch arbitrary ROMs by default.

Required checks: signature, checksum, platform/architecture, semantic version, manifest file list, target installation root, free disk space and rollback state. A patch cannot run a post-install script unless that script is part of a signed release and its purpose is disclosed.

ROM patches (IPS/BPS) are a separate user-authorized feature and must never ship copyrighted ROM images.
