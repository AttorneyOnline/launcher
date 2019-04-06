interface Manifest {
    /** Contains launcher-specific information. */
    launcher?: {
        /**
         * A Qt RCC file to be used for branding the launcher.
         *
         * Your RCC file can contain two files:
         *  - `logo.png` - 128px logo
         *  - `install_notice.html` - notice shown before an initial download
         */
        branding?: string;

        /**
         * The version of the launcher that should be used by this
         * manifest.
         */
        version: string;
    }

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

type Task = DownloadTask | DeleteTask | NoticeTask | MoveTask;

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
    target: string | string[];
}

interface NoticeTask {
    action: "notice";
    msg: string;

    /**
     * Specifies if this notice should only be shown if the launcher's version
     * is not equal to the one specified in the manifest (assuming there is a
     * version specified in the manifest).
     */
    versionCheck?: boolean;
}

interface MoveTask {
    action: "move";
    source: string;
    target: string;
}
