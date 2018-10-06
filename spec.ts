/** Checksums of an archive. There may be other checksum types in the future. */
interface Hash {
    md5: string;
}

/** Represents a single version of a component. */
interface Version {
    /**
     * SemVer-like versioning. Please keep versions monotonically increasing;
     * in case of a serious issue, you should roll forward, but the updater will
     * still try to roll back anyway if the latest version is older than the
     * installed version.
     */
    version: string;

    /** An optional update description formatted in Markdown. */
    description?: string;

    url: string;
    hash: Hash;

    /**
     * If updating from a specific previous version, an update archive with
     * update operations can be specified here.
     */
    updateFrom?: {
        [fromVersion: string]: {
            url: string;
            hash: Hash;
            operations: UpdateOperation[];
        }
    }
}

/** The manifest to be used for the program. */
interface ProgramManifest extends Version {
    /**
     * A version history - it may be possible in the future to downgrade to a
     * specific version.
     */
    otherVersions: { [version: string]: Version };
}

/** The manifest to be used for the base content. */
interface BaseContentManifest extends Version {
    /** A URL to the base content for use by webAO. */
    web?: string;
}

interface UpdateOperation {
    /** Specifies the timing of the operation: before or after the update. */
    type: "pre" | "post";

    /** Specifies the action to be performed on the target(s). */
    action: "delete";

    /**
     * Specifies a list of files or directories to perform the action on.
     * 
     * NOTE: when implementing, do not attempt to process relative paths that
     * escape the install directory!
     */
    targets: string[];
}
