import { motion, AnimatePresence } from "framer-motion";
import { ScanText } from "lucide-react";

function Tokenizer({ tokens, visibleCount }) {

  return (
    <div className="tokenizer-container">

      <div className="tokenizer-header">
        <div>
          <ScanText size={19} />
          <h3>LEXICAL TOKENIZATION</h3>
        </div>

        <span>
          {visibleCount}/{tokens.length} TOKENS
        </span>
      </div>

      <div className="tokenizer-input">
        <span>QUERY INPUT</span>
        <code>
          {tokens.join(" ")}
        </code>
      </div>

      <div className="token-grid">

        <AnimatePresence>
          {tokens.slice(0, visibleCount).map((token, index) => (

            <motion.div
              key={`${index}-${token}`}
              className="token-card"
              initial={{ opacity: 0, y: 15, scale: 0.9 }}
              animate={{ opacity: 1, y: 0, scale: 1 }}
              transition={{ duration: 0.3 }}
            >

              <span className="token-index">
                TOKEN {index + 1}
              </span>

              <h4>{token}</h4>

              <span className="token-type">
                {index === 0
                  ? "COMMAND CANDIDATE"
                  : "WHITESPACE TOKEN"}
              </span>

            </motion.div>

          ))}
        </AnimatePresence>

      </div>

      {visibleCount === tokens.length && tokens.length > 0 && (
        <div className="tokenizer-success">
          ✓ Tokenization completed — {tokens.length} tokens generated.
        </div>
      )}

    </div>
  );
}

export default Tokenizer;