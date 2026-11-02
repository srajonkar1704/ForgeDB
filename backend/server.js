const express = require("express");
const cors = require("cors");
const { spawn } = require("child_process");
const path = require("path");
const fs = require("fs");

const app = express();
const PORT = 5000;

// ForgeDB root directory
const PROJECT_ROOT = path.resolve(__dirname, "..");

// Actual C++ executable
const FORGEDB_EXECUTABLE = path.join(
    PROJECT_ROOT,
    "build",
    "forgedb_cli"
);

app.use(cors({
    origin: "http://localhost:5173"
}));

app.use(express.json({ limit: "10kb" }));

// Check whether the C++ executable exists
app.get("/api/status", (req, res) => {
    const executableExists = fs.existsSync(FORGEDB_EXECUTABLE);

    res.json({
        status: executableExists ? "READY" : "NOT_READY",
        engine: "ForgeDB C++",
        executable: executableExists
    });
});

// Execute a SQL query through the C++ engine
app.post("/api/query", (req, res) => {
    const { query } = req.body;

    if (typeof query !== "string" || !query.trim()) {
        return res.status(400).json({
            success: false,
            error: "A valid SQL query is required."
        });
    }

    const sql = query.trim();

    if (sql.length > 2000) {
        return res.status(400).json({
            success: false,
            error: "Query is too long."
        });
    }

    // Only allow commands currently supported by ForgeDB
    const command = sql.split(/\s+/)[0].toUpperCase();

    if (!["CREATE", "INSERT", "SELECT"].includes(command)) {
        return res.status(400).json({
            success: false,
            error: `Unsupported command: ${command}`
        });
    }

    if (!fs.existsSync(FORGEDB_EXECUTABLE)) {
        return res.status(500).json({
            success: false,
            error: "ForgeDB executable not found. Build the C++ project first."
        });
    }

    // Launch the actual C++ database executable
    const process = spawn(FORGEDB_EXECUTABLE, [], {
        cwd: PROJECT_ROOT
    });

    let stdout = "";
    let stderr = "";
    let timedOut = false;
    let responseSent = false;

    process.stdout.on("data", (data) => {
        stdout += data.toString();
    });

    process.stderr.on("data", (data) => {
        stderr += data.toString();
    });

    const timeout = setTimeout(() => {
        timedOut = true;
        process.kill("SIGKILL");
    }, 10000);

    process.on("error", (error) => {
    clearTimeout(timeout);

    if (responseSent) return;
    responseSent = true;

    return res.status(500).json({
        success: false,
        error: `Could not start ForgeDB: ${error.message}`
    });
});

process.on("close", (code) => {
    clearTimeout(timeout);

    if (responseSent) return;
    responseSent = true;

    if (timedOut) {
        return res.status(504).json({
            success: false,
            error: "Query execution timed out."
        });
    }

    res.json({
        success: code === 0,
        command,
        stdout: stdout.trim(),
        stderr: stderr.trim(),
        exitCode: code
    });
});
    // Send SQL to C++ through standard input
    process.stdin.write(sql + "\n");
    process.stdin.end();
});

app.listen(PORT, () => {
    console.log(`ForgeDB backend running at http://localhost:${PORT}`);
});
