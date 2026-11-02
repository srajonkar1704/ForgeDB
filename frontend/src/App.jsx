import { useState } from "react";

import {
  Database,
  Terminal,
  HardDrive,
  Layers,
  Activity,
  Play,
  Code2,
  Table2,
  ChevronRight,
} from "lucide-react";

import "./App.css";

import Tokenizer from "./components/Tokenizer";
import ExecutionPipeline from "./components/ExecutionPipeline";
import DataFlowAnimation from "./components/DataFlowAnimation";

// Backend API
const API_URL = "http://localhost:5000/api/query";


// ============================================================
// EXECUTION PATH CONFIGURATION
// ============================================================

function getExecutionPath(query) {

  const command = query.trim().split(/\s+/)[0]?.toUpperCase();

  const commonStages = [
    {
      id: "tokenizer",
      title: "TOKENIZER",
      description: "Breaking SQL into individual tokens",
    },
    {
      id: "parser",
      title: "PARSER",
      description: "Interpreting SQL command structure",
    },
    {
      id: "executor",
      title: "QUERY EXECUTOR",
      description: "Dispatching query operation",
    },
  ];

  const paths = {

    CREATE: [
      ...commonStages,
      {
        id: "create",
        title: "CREATE HANDLER",
        description: "Creating database table",
      },
      {
        id: "heapfile",
        title: "HEAP FILE",
        description: "Initializing table storage",
      },
      {
        id: "disk",
        title: "DISK",
        description: "Creating physical storage file",
      },
    ],

    INSERT: [
      ...commonStages,
      {
        id: "insert",
        title: "INSERT HANDLER",
        description: "Preparing database record",
      },
      {
        id: "heapfile",
        title: "HEAP FILE",
        description: "Locating available page",
      },
      {
        id: "pager",
        title: "PAGER",
        description: "Managing page access",
      },
      {
        id: "page",
        title: "PAGE",
        description: "Serializing and storing record",
      },
      {
        id: "disk",
        title: "DISK",
        description: "Writing page to storage",
      },
    ],

    SELECT: [
      ...commonStages,
      {
        id: "select",
        title: "SELECT HANDLER",
        description: "Initiating sequential scan",
      },
      {
        id: "heapfile",
        title: "HEAP FILE",
        description: "Accessing table storage",
      },
      {
        id: "pager",
        title: "PAGER",
        description: "Retrieving database pages",
      },
      {
        id: "page",
        title: "PAGE",
        description: "Reading and deserializing records",
      },
      {
        id: "results",
        title: "RESULT",
        description: "Returning retrieved records",
      },
    ],

  };

  return paths[command] || null;
}


// ============================================================
// DATA FLOW STAGE MAPPING
// ============================================================

const flowStageMap = {

  CREATE: {
    create: 0,
    heapfile: 1,
    disk: 2,
  },

  INSERT: {
    insert: 0,
    heapfile: 1,
    pager: 2,
    page: 3,
    disk: 4,
  },

  SELECT: {
    heapfile: 0,
    pager: 1,
    page: 2,
  },

};


// ============================================================
// STORAGE NODE COMPONENT
// ============================================================

function StorageNode({
  icon,
  title,
  subtitle,
  active,
}) {

  return (

    <div className={`storage-node ${active ? "active" : ""}`}>

      {icon}

      <h4>{title}</h4>

      <span>{subtitle}</span>

    </div>

  );

}


// ============================================================
// MAIN APPLICATION
// ============================================================

function App() {

  // ----------------------------------------------------------
  // STATE MANAGEMENT
  // ----------------------------------------------------------

  const [query, setQuery] = useState(
    "SELECT * FROM students;"
  );

  const [activeStage, setActiveStage] = useState(-1);

  const [completedStages, setCompletedStages] = useState([]);

  const [executionStages, setExecutionStages] = useState([]);

  const [tokens, setTokens] = useState([]);

  const [visibleTokens, setVisibleTokens] = useState(0);

  const [status, setStatus] = useState("IDLE");

  const [activeFlowStep, setActiveFlowStep] = useState(-1);

  const [completedFlowSteps, setCompletedFlowSteps] = useState([]);

  // Actual response from the C++ database through Express
  const [backendResult, setBackendResult] = useState(null);


  // ----------------------------------------------------------
  // HELPER FUNCTIONS
  // ----------------------------------------------------------

  const sleep = (ms) =>
    new Promise(resolve => setTimeout(resolve, ms));


  const isStageReached = (stageId) => {

    const index = executionStages.findIndex(
      stage => stage.id === stageId
    );

    return (
      index !== -1 &&
      (
        activeStage === index ||
        completedStages.includes(index)
      )
    );

  };


  // ----------------------------------------------------------
  // DATA FLOW CONTROLLER
  // ----------------------------------------------------------

  const animateFlowStage = async (index) => {

    setActiveFlowStep(index);

    await sleep(1200);

    setCompletedFlowSteps(previous => {

      if (previous.includes(index)) {
        return previous;
      }

      return [...previous, index];

    });

  };


  // ==========================================================
  // QUERY EXECUTION
  // ==========================================================

  const executeQuery = async () => {

    if (!query.trim()) {
      return;
    }

    const command = query.trim().split(/\s+/)[0].toUpperCase();

    const path = getExecutionPath(query);

    if (!path) {

      setStatus("UNSUPPORTED COMMAND");

      setBackendResult(null);

      return;

    }


    // --------------------------------------------------------
    // RESET PREVIOUS EXECUTION
    // --------------------------------------------------------

    setStatus("PROCESSING");

    setBackendResult(null);

    setExecutionStages(path);

    setActiveStage(-1);

    setCompletedStages([]);

    setVisibleTokens(0);

    setTokens([]);

    setActiveFlowStep(-1);

    setCompletedFlowSteps([]);


    // Generate tokens using whitespace splitting.
    const generatedTokens = query.trim().split(/\s+/);

    setTokens(generatedTokens);


    try {

      // ======================================================
      // PHASE 1: FRONTEND VISUALIZATION
      // ======================================================

      for (let i = 0; i < path.length; i++) {

        const currentStage = path[i];

        setActiveStage(i);


        // ----------------------------------------------------
        // TOKENIZER
        // ----------------------------------------------------

        if (currentStage.id === "tokenizer") {

          for (let j = 0; j < generatedTokens.length; j++) {

            await sleep(350);

            setVisibleTokens(j + 1);

          }

        }


        // ----------------------------------------------------
        // DATA FLOW AND STORAGE VISUALIZATION
        // ----------------------------------------------------

        else {

          const flowIndex =
            flowStageMap[command]?.[currentStage.id];


          if (flowIndex !== undefined) {

            await animateFlowStage(flowIndex);

          }

          else if (
            command === "SELECT" &&
            currentStage.id === "results"
          ) {

            // SELECT visualization:
            // Deserialization and Result Set.

            await animateFlowStage(3);

            await animateFlowStage(4);

          }

          else {

            await sleep(1200);

          }

        }


        // Mark pipeline stage completed.

        setCompletedStages(previous => {

          if (previous.includes(i)) {
            return previous;
          }

          return [...previous, i];

        });

        await sleep(300);

      }


      // Finish visualization.

      setActiveStage(-1);

      setActiveFlowStep(-1);


      // ======================================================
      // PHASE 2: REAL C++ DATABASE EXECUTION
      // ======================================================

      const response = await fetch(API_URL, {

        method: "POST",

        headers: {
          "Content-Type": "application/json",
        },

        body: JSON.stringify({
          query: query,
        }),

      });


      // Receive response from Express.

      const data = await response.json();

      setBackendResult(data);


      // Check actual database execution result.

      if (!response.ok || !data.success) {

        setStatus("ERROR");

        return;

      }


      // Actual C++ execution succeeded.

      setStatus("COMPLETED");

    }


    catch (error) {

      console.error("ForgeDB execution error:", error);

      setBackendResult({

        success: false,

        error: error.message,

      });

      setStatus("ERROR");

      setActiveStage(-1);

      setActiveFlowStep(-1);

    }

  };


  // ==========================================================
  // MAIN UI
  // ==========================================================

  return (

    <div className="dashboard">


      {/* ================================================== */}
      {/* SIDEBAR */}
      {/* ================================================== */}

      <aside className="sidebar">

        <div className="brand">

          <Database size={27} />

          <div>

            <h2>ForgeDB</h2>

            <span>DATABASE ENGINE</span>

          </div>

        </div>


        <div className="sidebar-section">

          <p>WORKSPACE</p>

          <div className="nav-item active">

            <Terminal size={18} />

            SQL Workspace

          </div>

          <div className="nav-item">

            <Database size={18} />

            Databases

          </div>

          <div className="nav-item">

            <Layers size={18} />

            Storage Engine

          </div>

          <div className="nav-item">

            <Activity size={18} />

            Execution Logs

          </div>

        </div>


        {/* DATABASE FILES */}

        <div className="database-list">

          <p>DATABASE FILES</p>

          <div className="file-item">

            <ChevronRight size={15} />

            <Database size={15} />

            students.heap

          </div>

          <div className="file-item">

            <ChevronRight size={15} />

            <Database size={15} />

            EMPLOYEES.heap

          </div>

        </div>


        <div className="sidebar-bottom">

          <div className="status-dot" />

          ENGINE READY

        </div>

      </aside>



      {/* ================================================== */}
      {/* MAIN CONTENT */}
      {/* ================================================== */}

      <main className="main-content">


        {/* HEADER */}

        <header className="topbar">

          <div>

            <h2>SQL Workspace</h2>

            <span>
              Interactive Database Environment
            </span>

          </div>

          <div className="engine-status">

            <span className="status-dot" />

            FORGEDB ONLINE

          </div>

        </header>



        {/* ================================================== */}
        {/* SQL EDITOR */}
        {/* ================================================== */}

        <section className="editor-section">

          <div className="section-heading">

            <div>

              <Code2 size={19} />

              <h3>SQL QUERY EDITOR</h3>

            </div>

            <span>SQL</span>

          </div>


          <textarea

            value={query}

            onChange={(e) => setQuery(e.target.value)}

            spellCheck="false"

            placeholder="Enter SQL query here..."

          />


          <div className="editor-footer">

            <span>

              {status === "PROCESSING"
                ? "Executing query..."
                : "Ready to execute"}

            </span>


            <button

              onClick={executeQuery}

              disabled={status === "PROCESSING"}

            >

              <Play size={16} />

              EXECUTE QUERY

            </button>

          </div>

        </section>



        {/* ================================================== */}
        {/* TOKENIZER */}
        {/* ================================================== */}

        <Tokenizer

          tokens={tokens}

          visibleCount={visibleTokens}

        />



        {/* ================================================== */}
        {/* QUERY PROCESSING PIPELINE */}
        {/* ================================================== */}

        <section className="pipeline-section">

          <div className="section-heading">

            <div>

              <Activity size={19} />

              <h3>QUERY EXECUTION PIPELINE</h3>

            </div>

            <span>{status}</span>

          </div>


          <ExecutionPipeline

            stages={executionStages}

            activeStage={activeStage}

            completedStages={completedStages}

          />

        </section>



        {/* ================================================== */}
        {/* DATA FLOW ANIMATION */}
        {/* ================================================== */}

        <DataFlowAnimation

          command={query.trim().split(/\s+/)[0]?.toUpperCase()}

          activeStep={activeFlowStep}

          completedSteps={completedFlowSteps}

        />



        {/* ================================================== */}
        {/* STORAGE ENGINE */}
        {/* ================================================== */}

        <section className="storage-section">

          <div className="section-heading">

            <div>

              <HardDrive size={19} />

              <h3>STORAGE ENGINE ARCHITECTURE</h3>

            </div>

          </div>


          <div className="storage-flow">

            <StorageNode

              icon={<Database />}

              title="HEAP FILE"

              subtitle="Record Management"

              active={isStageReached("heapfile")}

            />

            <div className="flow-arrow">→</div>


            <StorageNode

              icon={<Layers />}

              title="PAGER"

              subtitle="Page Management"

              active={isStageReached("pager")}

            />

            <div className="flow-arrow">→</div>


            <StorageNode

              icon={<Table2 />}

              title="PAGE"

              subtitle="4096 Bytes"

              active={isStageReached("page")}

            />

            <div className="flow-arrow">→</div>


            <StorageNode

              icon={<HardDrive />}

              title="DISK"

              subtitle="Persistent Storage"

              active={isStageReached("disk")}

            />

          </div>

        </section>



        {/* ================================================== */}
        {/* BOTTOM GRID */}
        {/* ================================================== */}

        <section className="bottom-grid">


          {/* PAGE VISUALIZER */}

          <div className="panel">

            <div className="section-heading">

              <Layers size={19} />

              <h3>PAGE VISUALIZER</h3>

            </div>


            <div className="page-info">

              <div>

                <span>PAGE SIZE</span>

                <h2>4096 B</h2>

              </div>

              <div>

                <span>RECORD SIZE</span>

                <h2>100 B</h2>

              </div>

              <div>

                <span>CAPACITY</span>

                <h2>40 ROWS</h2>

              </div>

            </div>


            <div className="page-slots">

              {Array.from({ length: 40 }).map((_, i) => (

                <div

                  key={i}

                  className={`page-slot ${
                    i < 3 ? "occupied" : ""
                  }`}

                >

                  {i < 3 ? `R${i + 1}` : ""}

                </div>

              ))}

            </div>


            <p className="page-caption">

              PAGE 0 — Illustrative initial state

            </p>

          </div>



          {/* ================================================== */}
          {/* EXECUTION OUTPUT */}
          {/* ================================================== */}

          <div className="panel">

            <div className="section-heading">

              <Terminal size={19} />

              <h3>EXECUTION OUTPUT</h3>

            </div>


            <div className="output">


              {/* IDLE */}

              {status === "IDLE" && (

                <p>
                  Waiting for query execution...
                </p>

              )}


              {/* PROCESSING */}

              {status === "PROCESSING" && (

                <p>
                  Processing query pipeline...
                </p>

              )}


              {/* SUCCESS */}

              {status === "COMPLETED" && backendResult && (

                <div>

                  <p className="success">

                    ✓ Query executed successfully

                  </p>


                  <p>

                    COMMAND: {backendResult.command}

                  </p>


                  <div className="terminal-output">

                    <pre>
                      {backendResult.stdout ||
                        "Query executed. No output returned."}
                    </pre>

                  </div>

                </div>

              )}


              {/* UNSUPPORTED COMMAND */}

              {status === "UNSUPPORTED COMMAND" && (

                <p className="error">

                  ✗ Unsupported SQL command.

                </p>

              )}


              {/* ERROR */}

              {status === "ERROR" && (

                <div>

                  <p className="error">

                    ✗ Query execution failed.

                  </p>


                  <pre className="terminal-error">

                    {backendResult?.error ||
                      backendResult?.stderr ||
                      "Unable to communicate with ForgeDB backend."}

                  </pre>

                </div>

              )}


            </div>

          </div>


        </section>



        {/* ================================================== */}
        {/* FOOTER */}
        {/* ================================================== */}

        <footer>

          <span>ForgeDB v1.0</span>

          <span>
            Educational Relational Database Engine
          </span>

          <span>
            Built with C++ and React
          </span>

        </footer>


      </main>

    </div>

  );

}

export default App;