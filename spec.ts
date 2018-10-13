interface Manifest {
    /**
     * A list of all versions of the package. The first version entry is the
     * latest version.
     */
    versions: Version[];
}

/** The manifest to be used for the base content. */
interface BaseManifest extends Manifest {
    /** A URL to the base content for use by webAO. */
    web?: string;
}

/** Represents a single version of a package. */
interface Version {
    version: string;
    prev?: string;

	executable?: string;

    full?: Task[];
    update?: Task[];
}

type Task = DownloadTask | DeleteTask;

interface DownloadTask {
    action: "dl";
    url: string;

    /** Represents a SHA-1 hash of a file. */
    hash?: string;
}

interface DeleteTask {
    action: "delete" | "deleteDir";

    /**
     * Specifies a target to perform the action on.
     * 
     * NOTE: when implementing, do not attempt to process relative paths that
     * escape the install directory!
     */
    targets: string[];
}