/**
 * @param {HTMLElement} node
 * @param {() => void} handler
 */
export function clickOutside(node, handler) {
  /** @param {MouseEvent} event */
  const onClick = (event) => {
    // If the click is outside the node, trigger the handler
    if (node && !node.contains(/** @type {Node} */ (event.target))) {
      handler();
    }
  };

  document.addEventListener("click", onClick, true);

  return {
    destroy() {
      document.removeEventListener("click", onClick, true);
    },
  };
}
