import { writable } from "svelte/store";

export const firstTimeUser = writable<boolean>(
  localStorage.getItem("firstTimeUser") !== "false",
);

firstTimeUser.subscribe((value) => {
  localStorage.setItem("firstTimeUser", String(value));
});
