export function clickOutside(node, handler) {
  const onClick = (event) => {
    // If the click is outside the node, trigger the handler
    if (node && !node.contains(event.target)) {
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
