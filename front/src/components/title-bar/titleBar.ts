export interface TitleBarProps {
  title?: string;
  mainContentScrolled?: boolean;
}

export const forceWindowsTitlebar: boolean = false;

export function getWindowType(): "web" | "windows" | "mac" | "unknown" {
  if (typeof window !== "undefined") {
    if (!window.saucer) return "web";

    const platform = (navigator.platform || "").toUpperCase();
    const userAgent = navigator.userAgent || "";

    // Windows
    if (
      forceWindowsTitlebar ||
      platform.includes("WIN") ||
      /Windows/i.test(userAgent)
    )
      return "windows";

    // Mac
    if (platform.includes("MAC") || /Macintosh|Mac OS X/i.test(userAgent))
      return "mac";
  }

  return "unknown";
}

/** @returns number of pixels that the title bar occupies depending on the platform */
export function getSafeTitlebarSpacePx(): number {
  switch (getWindowType()) {
    case "web":
    case "unknown":
      return 10;

    case "windows":
      return 43;

    case "mac":
      return 33;
  }
}
