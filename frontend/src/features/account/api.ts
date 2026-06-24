import { request } from "../../shared/api/client";
import type { Account, Subscription } from "../../shared/api/types";

export async function getAccount() {
  return request<Account>("/account");
}

export async function getSubscription() {
  return request<Subscription>("/subscription");
}

export async function updateTelegramChatId(telegramChatId: string) {
  return request<{ success: boolean }>("/account/telegram-chat-id", {
    method: "PUT",
    body: JSON.stringify({ telegramChatId }),
  });
}

export async function upgradeSubscription() {
  return request<{ subscription: Subscription; message: string }>("/subscription/upgrade", {
    method: "POST",
  });
}
