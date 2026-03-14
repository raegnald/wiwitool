const numberWords: Record<string, string> = {
  ONE: "1",
  TWO: "2",
  THREE: "3",
  FOUR: "4",
};

/**
 * Converts a text-based ratio string (e.g., "ONE_TWO") to a numeric ratio (e.g., "1:2").
 */
export function formatRatio(ratioString: string): string {
  if (!ratioString || !ratioString.includes("_")) return ratioString;

  return ratioString
    .split("_")
    .map((word) => numberWords[word] || word)
    .join(":");
}
