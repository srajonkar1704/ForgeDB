import { motion } from "framer-motion";

import {
  FileText,
  Database,
  Layers,
  HardDrive,
  Package,
  ArrowRight,
  Download,
  Table2
} from "lucide-react";


const flowConfigurations = {

  INSERT: [
    {
      id: "record",
      title: "RECORD",
      description: "Preparing row data",
      icon: FileText
    },
    {
      id: "heapfile",
      title: "HEAP FILE",
      description: "Locating available page",
      icon: Database
    },
    {
      id: "pager",
      title: "PAGER",
      description: "Managing page access",
      icon: Layers
    },
    {
      id: "page",
      title: "PAGE",
      description: "Serializing and storing row",
      icon: Package
    },
    {
      id: "disk",
      title: "DISK",
      description: "Persisting page data",
      icon: HardDrive
    }
  ],

  SELECT: [
    {
      id: "heapfile",
      title: "HEAP FILE",
      description: "Accessing table",
      icon: Database
    },
    {
      id: "pager",
      title: "PAGER",
      description: "Retrieving page",
      icon: Layers
    },
    {
      id: "page",
      title: "PAGE",
      description: "Reading stored records",
      icon: Package
    },
    {
      id: "deserialize",
      title: "DESERIALIZATION",
      description: "Reconstructing records",
      icon: Download
    },
    {
      id: "result",
      title: "RESULT SET",
      description: "Returning records",
      icon: Table2
    }
  ],

  CREATE: [
    {
      id: "handler",
      title: "CREATE HANDLER",
      description: "Processing table creation",
      icon: FileText
    },
    {
      id: "heapfile",
      title: "HEAP FILE",
      description: "Initializing storage",
      icon: Database
    },
    {
      id: "disk",
      title: "DISK",
      description: "Creating physical file",
      icon: HardDrive
    }
  ]

};


function DataFlowAnimation({
  command,
  activeStep,
  completedSteps
}) {

  const steps = flowConfigurations[command];

  if (!steps) return null;

  return (

    <section className="dataflow-section">

      <div className="dataflow-header">

        <div>
          <Package size={20} />

          <h3>
            {command} DATA FLOW
          </h3>
        </div>

        <span>STORAGE ENGINE OPERATIONS</span>

      </div>


      <div className="dataflow-track">

        {steps.map((step, index) => {

          const Icon = step.icon;

          const active = activeStep === index;

          const completed = completedSteps.includes(index);

          return (

            <div className="dataflow-item" key={step.id}>

              <motion.div

                className={`dataflow-node ${
                  active ? "active" : ""
                } ${completed ? "completed" : ""}`}

                animate={{
                  scale: active ? 1.08 : 1,
                  opacity: active || completed ? 1 : 0.45
                }}

                transition={{ duration: 0.35 }}

              >

                <Icon size={25} />

                <h4>{step.title}</h4>

                <p>{step.description}</p>

                {active && (

                  <motion.div

                    className="dataflow-progress"

                    initial={{ width: "0%" }}

                    animate={{ width: "100%" }}

                    transition={{ duration: 0.8 }}

                  />

                )}

              </motion.div>


              {index !== steps.length - 1 && (

                <div className="dataflow-arrow">

                  <motion.div

                    className="data-packet"

                    initial={{ opacity: 0, x: -10 }}

                    animate={{
                      opacity: completed ? 1 : 0,
                      x: completed ? 0 : -10
                    }}

                    transition={{ duration: 0.4 }}

                  >
                    ●

                  </motion.div>

                  <ArrowRight size={18} />

                </div>

              )}

            </div>

          );

        })}

      </div>


      <div className="dataflow-footer">

        <span>
          {activeStep === -1
            ? "Waiting for storage operation..."
            : `Processing stage ${activeStep + 1} of ${steps.length}`}
        </span>

        <span>
          {completedSteps.length}/{steps.length} COMPLETED
        </span>

      </div>

    </section>

  );

}

export default DataFlowAnimation;